assert(loadfile('tool/base.lua'))()
protectglobaltable(true)
local sources = assert(loadfile('sources.lua'))()
local env = require('env')
local configuration = require('build-config')
local reflectprocess = require('reflect-process')
local reflectindex = require('reflect-index')
local generate = require('generate')

local currenttarget = ...
currenttarget = currenttarget or 'all'

if _G.dryrun then
	function env.execute(str)
		print(str)
		return true
	end
end

local platform = configuration.platform
local libincludedir = configuration.libincludedir
local libstaticdir = configuration.libstaticdir
local libdynamicdir = configuration.libdynamicdir
local builddir = configuration.builddir
local outputdir = configuration.outputdir

-- define build steps neccesary to obtain various intermediate and output files
local function build_rprocess(entry)
	print('reflect', entry.target)
	if not env.makepath(entry.target) then
		return false
	end
	local suc, err = pcall(reflectprocess, {
		source = entry.source,
		target = entry.target,
		modulename = entry.modulename,
	})
	if not suc then
		print(err)
	end
	return suc
end

local function build_rindex(entry)
	print('rindex', entry.target)
	if not env.makepath(entry.target) then
		return false
	end
	local suc, err = pcall(reflectindex, {
		items = entry.items,
		target = entry.target,
		indexname = entry.indexname,
		prolog = entry.prolog,
		epilog = entry.epilog,
	})
	if not suc then
		print(err)
	end
	return suc
end

local function build_generate(entry)
	print('generate', entry.target)
	if not env.makepath(entry.target) then
		return false
	end
	local suc, err = pcall(generate, {
		target = entry.target,
		source = entry.source,
		info = entry.info,
	})
	if not suc then
		print(err)
	end
	return suc
end

local function build_obj(entry)
	print('compile', entry.target)
	return env.compile{
		target = entry.target,
		source = entry.source,
		incpath = {
			'src',
			libincludedir,
			'.',
		},
		macros = configuration.compilermacros,
		asmfile = string.gsub(entry.target, '.[^.]*$', '.asm'),
	}
end

local function build_exe(entry)
	print('link exe', entry.target)
	return env.link{
		target = entry.target,
		items = entry.items,
		libs = entry.libraries,
		libpath = {
			libstaticdir,
			outputdir,
		},
	}
end

local function build_dll(entry)
	print('link dll', entry.target)
	return env.link{
		target = entry.target,
		items = entry.items,
		libs = entry.libraries,
		libpath = {
			libstaticdir,
			outputdir,
		},
		dll = true,
	}
end

local function build_copy(entry)
	print('copy', entry.target)
	return env.copy(entry.source, entry.target)
end

local entrymap = {}
local entrylist = {}
--[[
build entry:
	target - actual path to the file
	dependencies - list of file names that must be built before this file
	autodep - method to automatically add dependencies to the file:
		autodep = 'native'
		autodep = 'reflect'
	build - build procedure for the file
	isdummy - the target does not correspond to an actual file
	if build = build_obj:
		source - actual path to the corresponding .cpp file
	if build = build_exe or build = build_dll:
		items - list of actual paths to .obj files
		libraries - list of library names
	if build = build_copy:
		source - actual path to the source file
	if build = build_rprocess:
		source - actual path to the corresponding .hpp file
		modulename - internal module name based on the path
	if build = build_rindex:
		items - list of actual paths to .r files
		indexname - variable name for the module index
		prolog - list of paths to .lua files to be executed before the modules
		epilog - list of paths to .lua files to be executed after the modules
	if build = build_generate:
		source - actual path to the corresponding .template file
		info - table to be passed to the template scripts
]]

local function register(entry, ...)
	assert(entry.target)
	table.append(entrylist, entry)
	assert(entrymap[entry.target] == nil)
	entrymap[entry.target] = entry
	local names = {...}
	for i = 1, select('#', ...) do
		assert(entrymap[names[i]] == nil)
		entrymap[names[i]] = entry
	end
end

register(
	{
		target = 'sources.lua',
	})

local reflection_targets = {}

-- special reflection nodes for native binaries
local rindex_items = {}
for i, rec in ipairs{
	{
		target = 'client-main',
		indexname = {'window', 'rindex'},
		prolog = {
			'luainit/baselib.lua',
			'luainit/object.lua',
			'luainit/ffipure.lua',
			'luainit/reflection.lua',
		},
		epilog = {
			'luainit/hostlib.lua',
			'luainit/main.lua',
		},
	},
	{
		target = 'renderer-d3d9',
		indexname = {'graphics', 'rindex'},
	},
	{
		target = 'renderer-gles',
		indexname = {'graphics', 'rindex'},
	},
} do
	rindex_items[rec.target] = {}
	local rindexdeps = {
		'sources.lua',
		'src/reflection.hpp',
		'src/common.hpp',
	}
	if rec.prolog then
		for i, entry in ipairs(rec.prolog) do
			register(
				{
					target = entry,
				})
			table.append(rindexdeps, entry)
		end
	end
	if rec.epilog then
		for i, entry in ipairs(rec.epilog) do
			register(
				{
					target = entry,
				})
			table.append(rindexdeps, entry)
		end
	end
	register(
		{
			target = 'reflection/' .. rec.target .. '/rindex.cpp',
			build = build_rindex,
			items = rindex_items[rec.target],
			indexname = rec.indexname,
			prolog = rec.prolog,
			epilog = rec.epilog,
			dependencies = rindexdeps,
		})
	table.append(reflection_targets,
		'reflection/' .. rec.target .. '/rindex.cpp')
	register(
		{
			target = 'src/' .. rec.target .. '/rindex.hpp',
			autodep = 'native',
		},
		rec.target .. '/rindex.hpp')
	register(
		{
			target = builddir .. '/reflection/' .. rec.target .. '/rindex.o',
			build = build_obj,
			source = 'reflection/' .. rec.target .. '/rindex.cpp',
		},
		'b//reflection/' .. rec.target .. '/rindex.o')
end

-- nodes for binaries themselves
local target_items = {
	['client-main'] = {
		builddir .. '/reflection/client-main/rindex.o',
	},
	['renderer-d3d9'] = {
		builddir .. '/reflection/renderer-d3d9/rindex.o',
	},
	['renderer-gles'] = {
		builddir .. '/reflection/renderer-gles/rindex.o',
	},
}

register(
	{
		target = outputdir .. '/client-main.exe',
		build = build_exe,
		items = target_items['client-main'],
		libraries = {
			'lua51',
			'libpng16',
			'z',
			'gdi32',
			'user32',
			-- 'libFLAC_dynamic',
		},
		dependencies = {
			'sources.lua',
			-- outputdir .. '/libFLAC_dynamic.dll',
		},
	},
	'o//client-main.exe')

register(
	{
		target = outputdir .. '/renderer-d3d9.dll',
		build = build_dll,
		items = target_items['renderer-d3d9'],
		libraries = {
			-- 'gdi32',
			-- 'd3d9',
			-- 'd3dx9',
		},
		dependencies = {
			'sources.lua',
		},
	},
	'o//renderer-d3d9.dll')

register(
	{
		target = outputdir .. '/renderer-gles.dll',
		build = build_dll,
		items = target_items['renderer-gles'],
		libraries = {
			-- 'gdi32',
			-- 'GLESv2',
			-- 'EGL',
		},
		dependencies = {
			'sources.lua',
			-- outputdir .. '/libGLESv2.dll',
			-- outputdir .. '/libEGL.dll',
		},
	},
	'o//renderer-gles.dll')

-- nodes for third-party libraries
for i, name in ipairs{
	'libEGL',
	'libGLESv2',
	'libFLAC_dynamic',
} do
	register(
		{
			target = libdynamicdir .. '/' .. name .. '.dll',
		})
	register(
		{
			target = outputdir .. '/' .. name .. '.dll',
			build = build_copy,
			source = libdynamicdir .. '/' .. name .. '.dll',
		},
		'o//' .. name .. '.dll')
end

-- Visual Studio project files
local generate_info = {
	entrylist = entrylist,
	entrymap = entrymap,
	sources = sources,
}

local vsproj_targets = {
	'reflection',
}

for i, filename in ipairs{
	'vs/client-main/client-main.vcxproj',
	'vs/client-main/client-main.vcxproj.filters',
	'vs/client-main/client-main.vcxproj.user',
} do
	register(
		{
			target = filename .. '.template',
		})
	register(
		{
			target = filename,
			build = build_generate,
			source = filename .. '.template',
			info = generate_info,
			dependencies = {
				'sources.lua',
			},
		})
	table.append(vsproj_targets,
		filename)
end

-- special nodes
register(
	{
		target = 'bin',
		isdummy = true,
		dependencies = {
			outputdir .. '/client-main.exe',
			outputdir .. '/renderer-d3d9.dll',
			outputdir .. '/renderer-gles.dll',
		},
	})
register(
	{
		target = 'reflection',
		isdummy = true,
		dependencies = reflection_targets,
	})
register(
	{
		target = 'vsproj',
		isdummy = true,
		dependencies = vsproj_targets,
	})
register(
	{
		target = 'all',
		isdummy = true,
		dependencies = {
			'bin',
			'vsproj',
		},
	})

-- fill the graph with the source file nodes, based on 'build-sources'
for i, unit in ipairs(sources) do
	if unit.type == 'native' then
		if not unit.noheader then
			register(
				{
					target = 'src/' .. unit.name .. '.hpp',
					autodep = 'native',
				},
				unit.name .. '.hpp')
		end
		if not unit.headeronly then
			register(
				{
					target = 'src/' .. unit.name .. '.cpp',
					autodep = 'native',
				},
				unit.name .. '.cpp')
		end
		if unit.target and not unit.headeronly then
			register(
				{
					target = builddir .. '/src/' .. unit.name .. '.o',
					build = build_obj,
					source = 'src/' .. unit.name .. '.cpp',
				},
				'b//src/' .. unit.name .. '.o')
			table.append(target_items[unit.target],
				builddir .. '/src/' .. unit.name .. '.o')
		end
		if unit.target and unit.reflect then
			register(
				{
					target = 'reflection/' .. unit.name .. '.r',
					build = build_rprocess,
					source = 'src/' .. unit.name .. '.hpp',
					modulename = unit.name,
					autodep = 'reflection',
				})
			table.append(reflection_targets,
				'reflection/' .. unit.name .. '.r')
			table.append(rindex_items[unit.target],
				'reflection/' .. unit.name .. '.r')
			register(
				{
					target = 'reflection/' .. unit.name .. '.r.cpp',
					dependencies = {
						'reflection/' .. unit.name .. '.r',
					},
				})
			table.append(reflection_targets,
				'reflection/' .. unit.name .. '.r.cpp')
			register(
				{
					target = builddir .. '/reflection/' .. unit.name .. '.r.o',
					build = build_obj,
					source = 'reflection/' .. unit.name .. '.r.cpp',
				},
				'b//reflection/' .. unit.name .. '.r.o')
			table.append(target_items[unit.target],
				builddir .. '/reflection/' .. unit.name .. '.r.o')
		end
	end
end

local function autodep_native(entry)
	local file = io.open(env.path(entry.target), 'r')
	if file then
		for line in file:lines() do
			local filename = string.match(line, '#%s*include%s*<(.*)>')
			if filename then
				local depentry = entrymap[filename]
				if depentry then
					table.append(entry.dependencies, depentry.target)
				end
			end
		end
		file:close()
	end
end

local function autodep_reflection(entry)
	local file = io.open(env.path(entry.source), 'r')
	if file then
		for line in file:lines() do
			local filename = string.match(line, '#%s*include%s*<(.*)>')
			if filename then
				local modulename = string.match(filename, '^(.*)%.') or filename
				local rname = 'reflection/' .. modulename .. '.r'
				local depentry = entrymap[rname]
				if depentry then
					table.append(entry.dependencies, depentry.target)
				end
			end
		end
		file:close()
	end
end

-- fill the node dependency edges
for i, entry in ipairs(entrylist) do
	if not entry.dependencies then
		entry.dependencies = {}
	end
	-- compile-type nodes link to their sources
	if entry.source then
		table.append(entry.dependencies, entry.source)
	end
	if entry.items then
		for i, item in ipairs(entry.items) do
			table.append(entry.dependencies, item)
		end
	end
	-- source-type nodes link to their #includes
	if entry.autodep == 'native' then
		autodep_native(entry)
	elseif entry.autodep == 'reflection' then
		autodep_reflection(entry)
	end
end

-- for debugging purposes, it is possible to print out the full graph
-- without building anything
if _G.printgraph then
	for i, entry in ipairs(entrylist) do
		print(entry.target .. ':')
		for j, dep in ipairs(entry.dependencies) do
			print('', dep)
		end
	end
	return
end

local function buildentry(entry, force)
	entry.isactive = true
	local deptime = 0
	for i, dep in ipairs(entry.dependencies) do
		local depentry = assert(entrymap[dep])
		if not depentry.time then
			if depentry.isactive then
				error('circular dependency')
			end
			buildentry(depentry, _G.force)
			assert(depentry.time)
		end
		if depentry.time > deptime then
			deptime = depentry.time
		end
	end
	local targettime = env.getfiletime(entry.target)
	if force or not targettime or targettime < deptime then
		if entry.build then
			if not entry.build(entry) then
				error('build failed: ' .. entry.target)
			end
			local newtime = env.getfiletime(entry.target)
			if newtime then
				targettime = newtime
			else
				targettime = deptime
			end
		else
			targettime = deptime
		end
	end
	entry.time = targettime
end

buildentry(
	assert(entrymap[currenttarget], 'invalid target: ' .. currenttarget),
	true)
