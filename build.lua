assert(loadfile('base.lua'))()
local env = require('env')
local sources = require('build-sources')

local currenttarget = ...
currenttarget = currenttarget or 'all'

-- additional arguments:
--   platform = 'win32'|'win64'
--   configuration = 'debug'
--   dryrun = true|false

local ffi = require('ffi')

local platform = {
	['win32'] = true,
	['win64'] = true,
}
if _G.platform then
	platform = platform[_G.platform] and _G.platform or error('unknown platform')
else
	platform = ({
		['Windows-x86'] = 'win32',
		['Windows-x64'] = 'win64',
	})[ffi.os .. '-' .. ffi.arch] or error('unknown platform')
end

local configuration = {
	['debug'] = {
		tag = 'debug',
		debug = true,
		compilermacros = {
			MIST_DEBUG = true,
		},
	},
	['release'] = {
		tag = 'release',
		debug = false,
		compilermacros = {
		},
	},
}
if _G.configuration then
	configuration = configuration[_G.configuration] or
		error('unknown configuration')
else
	configuration = configuration['debug']
end

if not not _G.dryrun then
	function env.execute(str)
		print(str)
		return true
	end
end

local builddir = 'build/l-' .. platform .. '-' .. configuration.tag
local outputdir = 'output/bin-l-' .. platform .. '-' .. configuration.tag
local luacpath = builddir .. '/luac.exe'
currenttarget = string.gsub(currenttarget, '$b', builddir)
currenttarget = string.gsub(currenttarget, '$o', outputdir)

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

function env.luac(t)
	return
		env.makepath(t.target) and
		env.execute(env.path(luacpath) .. ' \z
			"' .. env.path(t.target) .. '" \z
			"' .. env.path(t.source) ..  '" \z
			' .. t.flags)
end

local function build_lua(entry)
	print('luac', entry.target)
	return env.luac{
		target = entry.target,
		source = entry.source,
		flags = configuration.debug and '' or 'd',
	}
end

local function build_methodlist(entry)
	print('methodlist', entry.target)
	return
		env.makepath(entry.target) and
		env.lua{
			vars = {
				compactffi = not configuration.debug,
				fileprefix = env.path(entry.target),
				structname = entry.structname,
			},
			script = 'bind.lua',
			args = entry.items,
		}
end

local function build_luainit(entry)
	print('luainit', entry.target)
	return
		env.makepath(entry.target) and
		env.lua{
			vars = {
				structname = 'luainit',
				fileprefix = env.path(entry.target),
			},
			script = 'embed.lua',
			args = entry.items,
		}
end

local function build_cpp(entry)
	print('compile', entry.target)
	return env.compile{
		target = entry.target,
		source = entry.source,
		incpath = builddir,
		macros = configuration.compilermacros,
	}
end

local function build_exe(entry)
	print('link exe', entry.target)
	return env.link{
		target = entry.target,
		items = entry.items,
		libs = entry.libraries,
		libpath = {
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
			outputdir,
		},
		dll = true,
	}
end

local function build_copy(entry)
	print('copy', entry.target)
	return env.copy(entry.source, entry.target)
end

local targets = {}

table.append(targets, {
	target = 'luac.cpp',
	issource = true,
	autodep = true,
})
table.append(targets, {
	build = build_cpp,
	target = builddir .. '/luac.o',
	source = 'luac.cpp',
})
table.append(targets, {
	build = build_exe,
	target = luacpath,
	items = {
		builddir .. '/luac.o'
	},
	libraries = {
		'luajit-' .. platform,
	},
})

local methodlist_items = {
	['client-console'] = {},
	['client-main'] = {},
	['renderer-d3d9'] = {},
	['renderer-gles'] = {},
}
for i, target in ipairs{
	'client-console',
	'client-main',
	'renderer-d3d9',
	'renderer-gles',
} do
	local targetname = string.gsub(target, '%-', '_')
	table.append(targets, {
		build = build_methodlist,
		target = builddir .. '/' .. target .. '/methodlist',
		items = methodlist_items[target],
		structname = targetname .. '_methodlist',
	})
	table.append(targets, {
		target = builddir .. '/' .. target .. '/methodlist.cpp',
		dependencies = {
			builddir .. '/' .. target .. '/methodlist',
		},
	})
	table.append(targets, {
		target = builddir .. '/' .. target .. '/methodlist.hpp',
		dependencies = {
			builddir .. '/' .. target .. '/methodlist',
		},
	})
	table.append(targets, {
		target = target .. '/methodlist.cpp',
		dependencies = {
			builddir .. '/' .. target .. '/methodlist.cpp',
		},
	})
	table.append(targets, {
		target = target .. '/methodlist.hpp',
		dependencies = {
			builddir .. '/' .. target .. '/methodlist.hpp',
		},
	})
	table.append(targets, {
		build = build_cpp,
		target = builddir .. '/' .. target .. '/methodlist.o',
		source = builddir .. '/' .. target .. '/methodlist.cpp',
	})
	table.append(targets, {
		target = builddir .. '/' .. target .. '/methodlist.lua',
		dependencies = {
			builddir .. '/' .. target .. '/methodlist',
		},
	})
end

for i, target in ipairs{
	'client-console',
	'client-main',
} do
	table.append(targets, {
		build = build_lua,
		target = builddir .. '/' .. target .. '/methodlist.lb',
		source = builddir .. '/' .. target .. '/methodlist.lua',
		dependencies = {
			luacpath,
		},
	})
	table.append(targets, {
		build = build_luainit,
		target = builddir .. '/' .. target .. '/luainit',
		items = {
			builddir .. '/luainit/main.lb',
			builddir .. '/luainit/baselib.lb',
			builddir .. '/luainit/object.lb',
			builddir .. '/luainit/ffipure.lb',
			builddir .. '/' .. target .. '/methodlist.lb',
			builddir .. '/luainit/hostlib.lb',
		},
	})
	table.append(targets, {
		target = builddir .. '/' .. target .. '/luainit.cpp',
		dependencies = {
			builddir .. '/' .. target .. '/luainit',
		},
	})
	table.append(targets, {
		target = builddir .. '/' .. target .. '/luainit.hpp',
		dependencies = {
			builddir .. '/' .. target .. '/luainit',
		},
	})
	table.append(targets, {
		target = target .. '/luainit.cpp',
		dependencies = {
			builddir .. '/' .. target .. '/luainit.cpp',
		},
	})
	table.append(targets, {
		target = target .. '/luainit.hpp',
		dependencies = {
			builddir .. '/' .. target .. '/luainit.hpp',
		},
	})
	table.append(targets, {
		build = build_cpp,
		target = builddir .. '/' .. target .. '/luainit.o',
		source = builddir .. '/' .. target .. '/luainit.cpp',
	})
end

for i, target in ipairs{
	'renderer-d3d9',
	'renderer-gles',
} do
	table.append(targets, {
		build = build_lua,
		target = builddir .. '/' .. target .. '/methodlist.lb',
		source = builddir .. '/' .. target .. '/methodlist.lua',
		dependencies = {
			luacpath,
		},
	})
	table.append(targets, {
		build = build_luainit,
		target = builddir .. '/' .. target .. '/luainit',
		items = {
			builddir .. '/' .. target .. '/methodlist.lb',
		},
	})
	table.append(targets, {
		target = builddir .. '/' .. target .. '/luainit.cpp',
		dependencies = {
			builddir .. '/' .. target .. '/luainit',
		},
	})
	table.append(targets, {
		target = builddir .. '/' .. target .. '/luainit.hpp',
		dependencies = {
			builddir .. '/' .. target .. '/luainit',
		},
	})
	table.append(targets, {
		target = target .. '/luainit.cpp',
		dependencies = {
			builddir .. '/' .. target .. '/luainit.cpp',
		},
	})
	table.append(targets, {
		target = target .. '/luainit.hpp',
		dependencies = {
			builddir .. '/' .. target .. '/luainit.hpp',
		},
	})
	table.append(targets, {
		build = build_cpp,
		target = builddir .. '/' .. target .. '/luainit.o',
		source = builddir .. '/' .. target .. '/luainit.cpp',
	})
end

local target_items = {
	['client-console'] = {
		builddir .. '/client-console/luainit.o',
		builddir .. '/client-console/methodlist.o',
	},
	['client-main'] = {
		builddir .. '/client-main/luainit.o',
		builddir .. '/client-main/methodlist.o',
	},
	['renderer-d3d9'] = {
		builddir .. '/renderer-d3d9/luainit.o',
		builddir .. '/renderer-d3d9/methodlist.o',
	},
	['renderer-gles'] = {
		builddir .. '/renderer-gles/luainit.o',
		builddir .. '/renderer-gles/methodlist.o',
	},
}

table.append(targets, {
	build = build_exe,
	target = outputdir .. '/client-console.exe',
	items = target_items['client-console'],
	libraries = {
		'luajit-' .. platform,
		'png-' .. platform,
		'z',
	},
})
table.append(targets, {
	build = build_exe,
	target = outputdir .. '/client-main.exe',
	items = target_items['client-main'],
	libraries = {
		'luajit-' .. platform,
		'png-' .. platform,
		'gdi32',
		'z',
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
		outputdir .. '/d3dcompiler_47.dll',
		outputdir .. '/libEGL.dll',
		outputdir .. '/libGLESv2.dll',
	},
})
for i, name in ipairs{
	'd3dcompiler_47',
	'libEGL',
	'libGLESv2',
} do
	table.append(targets, {
		target = name .. '-' .. platform .. '.dll',
		issource = true,
	})
	table.append(targets, {
		build = build_copy,
		target = outputdir .. '/' .. name .. '.dll',
		source = name .. '-' .. platform .. '.dll',
	})
end

table.append(targets, {
	target = 'all',
	dependencies = {
		outputdir .. '/client-console.exe',
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
		outputdir .. '/client-console.exe',
		outputdir .. '/client-main.exe',
		outputdir .. '/renderer-d3d9.dll',
		outputdir .. '/renderer-gles.dll',
		outputdir .. '/d3dcompiler_47.dll',
		outputdir .. '/libEGL.dll',
		outputdir .. '/libGLESv2.dll',
	},
})

for i, unit in ipairs(sources) do
	if unit.type == 'native' then
		if not unit.noheader then
			table.append(targets, {
				target = unit.name .. '.hpp',
				issource = true,
				autodep = true,
			})
		end
		if not unit.headeronly then
			table.append(targets, {
				target = unit.name .. '.cpp',
				issource = true,
				autodep = true,
			})
			table.append(targets, {
				build = build_cpp,
				target = builddir .. '/' .. unit.name .. '.o',
				source = unit.name .. '.cpp',
			})
		end
		for use in string.gmatch(unit.use, '[^;]+') do
			if not unit.headeronly then
				table.append(target_items[use],
					builddir .. '/' .. unit.name .. '.o')
			end
		end
		if unit.methodlist then
			for mluse in string.gmatch(unit.methodlist, '[^;]+') do
				table.append(methodlist_items[mluse], unit.name .. '.hpp')
			end
		end
	elseif unit.type == 'luainit' then
		table.append(targets, {
			target = unit.name .. '.lua',
			issource = true,
		})
		table.append(targets, {
			build = build_lua,
			target = builddir .. '/' .. unit.name .. '.lb',
			source = unit.name .. '.lua',
			dependencies = {
				luacpath,
			},
		})
	end
end

for i, entry in ipairs(targets) do
	targets[entry.target] = entry
end

-- fill dependencies
for i, entry in ipairs(targets) do
	if not entry.dependencies then
		entry.dependencies = {}
	end
	if entry.source then
		table.append(entry.dependencies, entry.source)
	end
	if entry.objects then
		for i, obj in ipairs(entry.objects) do
			table.append(entry.dependencies, object)
		end
	end
	if entry.items then
		for i, item in ipairs(entry.items) do
			table.append(entry.dependencies, item)
		end
	end
	if entry.autodep then
		local f, err = io.open(env.path(entry.target), 'r')
		if f then
			for line in f:lines() do
				local fn = string.match(line, '#%s*include%s*<(.*)>')
				if fn and targets[fn] then
					table.append(entry.dependencies, fn)
				end
			end
		end
	end
end

-- add backward links to the dependency graph
-- do
	-- for i, entry in ipairs(targets) do
		-- entry.follows = {}
	-- end
	-- for i, entry in ipairs(targets) do
		-- for i, dep in ipairs(entry.dependencies) do
			-- table.append(targets[dep].follows, entry.target)
		-- end
	-- end
-- end

-- sort targets according to dependencies
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
end

for i, entry in ipairs(targets) do
	targets[entry.target] = entry
end

if _G.printgraph then
	for i, entry in ipairs(targets) do
		print(entry.target .. ':')
		for j, dep in ipairs(entry.dependencies) do
			print('', dep)
		end
	end
	return
end

-- mark the targets to build
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

-- detect changes and propagate them through the graph
for i, entry in ipairs(targets) do
	if entry.issource then
		local time = env.getfiletime(entry.target)
		if time and
			(not buildstate.timemap[entry.target] or
				time > buildstate.timemap[entry.target])
		then
			print('changed:', entry.target)
			buildstate.updatemap[entry.target] = true
		end
	end
	if entry.alwaysbuild or entry.target == currenttarget then
		buildstate.updatemap[entry.target] = true
	end
	for j, dep in ipairs(entry.dependencies) do
		local depentry = targets[dep]
		if buildstate.updatemap[dep] then
			buildstate.updatemap[entry.target] = true
			break
		end
	end
end

savebuildstate()

-- build marked targets
do
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
			end
		end
		buildstate.timemap[entry.target] = env.getfiletime(entry.target)
	end
end
savebuildstate()
