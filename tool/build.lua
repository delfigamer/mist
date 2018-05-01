assert(loadfile('tool/base.lua'))()
protectglobaltable(true)
local sources = assert(loadfile('sources.lua'))()
local env = require('env')
local configuration = require('build-config')
local reflectprocess = require('reflect-process')
local reflectindex = require('reflect-index')

local currenttarget = ...
currenttarget = currenttarget or 'all'

if _G.dryrun then
	function env.execute(str)
		print(str)
		return true
	end
end

local platform = configuration.platform
local srcdir = configuration.srcdir
local libincludedir = configuration.libincludedir
local libstaticdir = configuration.libstaticdir
local libdynamicdir = configuration.libdynamicdir
local builddir = configuration.builddir
local outputdir = configuration.outputdir
currenttarget = string.gsub(currenttarget, '$b', builddir)
currenttarget = string.gsub(currenttarget, '$o', outputdir)

-- define build steps neccesary to obtain various intermediate and output files
local function build_clean(entry)
	for i, dep in ipairs(entry.directories) do
		print('rmdir', dep)
		env.rmdir(dep)
	end
	for i, dep in ipairs(entry.files) do
		print('rm', dep)
		env.rm(dep)
	end
	return true
end

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

local function build_cpp(entry)
	print('compile', entry.target)
	return env.compile{
		target = entry.target,
		source = entry.source,
		incpath = {
			srcdir,
			libincludedir,
			builddir,
			'.',
		},
		macros = configuration.compilermacros,
		asmfile =
			configuration.saveintermediates and
			string.gsub(entry.target, '.[^.]*$', '.asm'),
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

-- construct the build graph
local targets = {}

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
			'luainit/reflection.lua'},
		epilog = {
			'luainit/hostlib.lua',
			'luainit/main.lua'}},
	{
		target = 'renderer-d3d9',
		indexname = {'graphics', 'rindex'}},
	{
		target = 'renderer-gles',
		indexname = {'graphics', 'rindex'}},
} do
	rindex_items[rec.target] = {}
	local rindexdeps = {
		'reflection.hpp',
		'common.hpp'}
	if rec.prolog then
		for i, entry in ipairs(rec.prolog) do
			table.append(targets, {
				target = entry,
				issource = true,
			})
			table.append(rindexdeps, entry)
		end
	end
	if rec.epilog then
		for i, entry in ipairs(rec.epilog) do
			table.append(targets, {
				target = entry,
				issource = true,
			})
			table.append(rindexdeps, entry)
		end
	end
	table.append(targets, {
		build = build_rindex,
		target = builddir .. '/' .. rec.target .. '/rindex.cpp',
		items = rindex_items[rec.target],
		indexname = rec.indexname,
		prolog = rec.prolog,
		epilog = rec.epilog,
		dependencies = rindexdeps,
	})
	table.append(targets, {
		target = srcdir .. '/' .. rec.target .. '/rindex.hpp',
		issource = true,
		autodep = 'native',
	})
	table.append(targets, {
		target = rec.target .. '/rindex.hpp',
		dependencies = {
			srcdir .. '/' .. rec.target .. '/rindex.hpp',
		},
	})
	table.append(targets, {
		build = build_cpp,
		target = builddir .. '/' .. rec.target .. '/rindex.o',
		source = builddir .. '/' .. rec.target .. '/rindex.cpp',
	})
end

-- nodes for binaries themselves
local target_items = {
	['client-main'] = {
		builddir .. '/client-main/rindex.o',
	},
	['renderer-d3d9'] = {
		builddir .. '/renderer-d3d9/rindex.o',
	},
	['renderer-gles'] = {
		builddir .. '/renderer-gles/rindex.o',
	},
}

table.append(targets, {
	build = build_exe,
	target = outputdir .. '/client-main.exe',
	items = target_items['client-main'],
	libraries = {
		'luajit-' .. platform,
		'png-' .. platform,
		'gdi32',
		'z',
		'libFLAC_dynamic',
	},
	dependencies = {
		outputdir .. '/libFLAC_dynamic.dll',
	},
})

table.append(targets, {
	build = build_dll,
	target = outputdir .. '/renderer-d3d9.dll',
	items = target_items['renderer-d3d9'],
	libraries = {
		'gdi32',
		'd3d9',
		'd3dx9',
	},
})

table.append(targets, {
	build = build_dll,
	target = outputdir .. '/renderer-gles.dll',
	items = target_items['renderer-gles'],
	libraries = {
		'gdi32',
		'GLESv2',
		'EGL',
	},
	dependencies = {
		outputdir .. '/libEGL.dll',
		outputdir .. '/libGLESv2.dll',
	},
})

-- nodes for third-party libraries
for i, name in ipairs{
	'libEGL',
	'libGLESv2',
	'libFLAC_dynamic',
} do
	table.append(targets, {
		target = libdynamicdir .. '/' .. name .. '-' .. platform .. '.dll',
		issource = true,
	})
	table.append(targets, {
		build = build_copy,
		target = outputdir .. '/' .. name .. '.dll',
		source = libdynamicdir .. '/' .. name .. '-' .. platform .. '.dll',
	})
end

-- special nodes
table.append(targets, {
	target = 'all',
	dependencies = {
		outputdir .. '/client-main.exe',
		outputdir .. '/renderer-d3d9.dll',
		outputdir .. '/renderer-gles.dll',
	},
})

table.append(targets, {
	build = build_clean,
	target = 'clean',
	alwaysbuild = true,
	directories = {
		builddir,
	},
	files = {
		outputdir .. '/client-main.exe',
		outputdir .. '/renderer-d3d9.dll',
		outputdir .. '/renderer-gles.dll',
		outputdir .. '/libEGL.dll',
		outputdir .. '/libGLESv2.dll',
	},
})

-- fill the graph with the source file nodes, based on 'build-sources'
for i, unit in ipairs(sources) do
	if unit.type == 'native' then
		if not unit.noheader then
			table.append(targets, {
				target = srcdir .. '/' .. unit.name .. '.hpp',
				issource = true,
				autodep = 'native',
			})
			table.append(targets, {
				target = unit.name .. '.hpp',
				dependencies = {
					srcdir .. '/' .. unit.name .. '.hpp',
				},
			})
		end
		if not unit.headeronly then
			table.append(targets, {
				target = srcdir .. '/' .. unit.name .. '.cpp',
				issource = true,
				autodep = 'native',
			})
			table.append(targets, {
				target = unit.name .. '.cpp',
				dependencies = {
					srcdir .. '/' .. unit.name .. '.cpp',
				},
			})
		end
		if unit.target and not unit.headeronly then
			table.append(targets, {
				build = build_cpp,
				target = builddir .. '/' .. unit.name .. '.o',
				source = srcdir .. '/' .. unit.name .. '.cpp',
			})
			table.append(target_items[unit.target],
				builddir .. '/' .. unit.name .. '.o')
		end
		if unit.target and unit.reflect then
			table.append(targets, {
				build = build_rprocess,
				target = builddir .. '/' .. unit.name .. '.r',
				source = srcdir .. '/' .. unit.name .. '.hpp',
				modulename = unit.name,
				autodep = 'reflection',
			})
			table.append(rindex_items[unit.target],
				builddir .. '/' .. unit.name .. '.r')
			table.append(targets, {
				target = builddir .. '/' .. unit.name .. '.r.cpp',
				dependencies = {
					builddir .. '/' .. unit.name .. '.r',
				},
			})
			table.append(targets, {
				build = build_cpp,
				target = builddir .. '/' .. unit.name .. '.r.o',
				source = builddir .. '/' .. unit.name .. '.r.cpp',
			})
			table.append(target_items[unit.target],
				builddir .. '/' .. unit.name .. '.r.o')
		end
	end
end

-- provide the name-to-node mapping
for i, entry in ipairs(targets) do
	targets[entry.target] = entry
end

-- fill the node dependency edges
for i, entry in ipairs(targets) do
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
		local file = io.open(env.path(entry.target), 'r')
		if file then
			for line in file:lines() do
				local target = string.match(line, '#%s*include%s*<(.*)>')
				if target and targets[target] then
					table.append(entry.dependencies, target)
				end
			end
			file:close()
		end
	elseif entry.autodep == 'reflection' then
		local file = io.open(env.path(entry.source), 'r')
		if file then
			for line in file:lines() do
				local target = string.match(line, '#%s*include%s*<(.*)>')
				if target then
					local modulename = string.match(target, '^(.*)%.') or target
					local rpath = builddir .. '/' .. modulename .. '.r'
					if targets[rpath] then
						table.append(entry.dependencies, rpath)
					end
				end
			end
			file:close()
		end
	end
end

-- sort the list, so that dependent nodes are placed after their dependencies
do
	local function preceding(entry)
		local r = {}
		for i, dep in ipairs(entry.dependencies) do
			table.append(r, targets[dep])
		end
		return r
	end
	local result, leftover = table.weak_sort(targets, preceding)
	if result then
		targets = result
	else
		for i, entry in ipairs(current) do
			print(string.format('target %s:', entry.target))
			for j, dep in ipairs(entry.dependencies) do
				print(string.format('\t%s', dep))
			end
		end
		error('cannot resolve dependency graph')
	end
	for i, entry in ipairs(targets) do
		targets[entry.target] = entry
	end
end

-- for debugging purposes, it is possible to print out the full graph
-- without building anything
if _G.printgraph then
	for i, entry in ipairs(targets) do
		print(entry.target .. ':')
		for j, dep in ipairs(entry.dependencies) do
			print('', dep)
		end
	end
	return
end

-- starting with the final target,
-- propagate the 'is needed' mark backwards through the graph
do
	local next = {targets[currenttarget]}
	if not next[1] then
		error('invalid current target')
	end
	next[1].marked = true
	while #next > 0 do
		local entry = table.pop(next)
		for i, dep in ipairs(entry.dependencies) do
			local depentry = targets[dep]
			if not depentry.marked then
				depentry.marked = true
				table.append(next, depentry)
			end
		end
	end
end

-- load the state of previous build
local buildstate
if not _G.cleanstate then
	local f = loadfile(env.path(builddir .. '/buildstate.lua'), 't', '', {})
	local suc, ret = pcall(f)
	if type(ret) == 'table' and
		type(ret.timemap) == 'table' and
		type(ret.updatemap) == 'table'
	then
		buildstate = ret
	end
end
buildstate = buildstate or {
	timemap = {},
	updatemap = {},
}

local function savebuildstate()
	env.makepath(builddir .. '/buildstate.lua')
	local f, err = io.open(env.path(builddir .. '/buildstate.lua'), 'w')
	if not f then
		return
	end
	f:write('return {\n\ttimemap = {\n')
	for k, v in pairs(buildstate.timemap) do
		f:write(string.format('\t\t[%q] = %s,\n',
			k, v))
	end
	f:write('\t},\n\tupdatemap = {\n')
	for k, v in pairs(buildstate.updatemap) do
		f:write(string.format('\t\t[%q] = %s,\n',
			k, v))
	end
	f:write('\t},\n}\n')
	f:close()
end

-- starting with changed source files,
-- propagate the 'is changed' mark forward through the graph
for i, entry in ipairs(targets) do
	if entry.issource then
		local time = env.getfiletime(entry.target)
		if time then
			if
				(not buildstate.timemap[entry.target] or
					time > buildstate.timemap[entry.target])
			then
				print('changed:', entry.target)
				buildstate.updatemap[entry.target] = true
				buildstate.timemap[entry.target] = time
			end
		else
			print('source file unavailable:', entry.target)
			buildstate.updatemap[entry.target] = true
			buildstate.timemap[entry.target] = nil
		end
	end
	if entry.alwaysbuild or entry.target == currenttarget then
		buildstate.updatemap[entry.target] = true
	end
	for j, dep in ipairs(entry.dependencies) do
		if buildstate.updatemap[dep] then
			buildstate.updatemap[entry.target] = true
			break
		end
	end
end

savebuildstate()

-- build nodes that are marked as both 'needed' and 'changed'
for i, entry in ipairs(targets) do
	if entry.marked then
		if (buildstate.updatemap[entry.target] or entry.alwaysbuild) and
			entry.build
		then
			if not entry:build() then
				print('build failed', entry.target)
				break
			end
		end
		if not _G.dryrun then
			buildstate.updatemap[entry.target] = nil
			savebuildstate()
		end
	end
end
