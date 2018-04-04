local modname = ...
local env = require('env')
local format = require('reflect-format')
local rcache = require('reflect-cache')

-- entry point: reflect(fenv)
-- fenv fields:
--   items
--   target
--   indexname
--   prolog
--   epilog

local function loadluapart(source, marker)
	local part = {
		chunk = string.dump(assert(loadfile(env.path(source)))),
		name = source}
	part[marker] = true
	return part
end

local function loadluapartlist(partlist, sourcelist, marker)
	if sourcelist then
		for i, source in ipairs(sourcelist) do
			table.append(partlist, loadluapart(source, marker))
		end
	end
end

local function rindex(fenv)
	local modulelist = {}
	local modulemap = {}

	for i, filename in ipairs(fenv.items) do
		local moduledef = assert(rcache.tryloadmodulebypath(filename))
		table.append(modulelist, moduledef)
		modulemap[moduledef.name] = moduledef
	end

	do
		local result, part = table.weak_sort(
			modulelist,
			function(b)
				local ret = {}
				for i, inc in ipairs(b.includes) do
					ret[i] = modulemap[inc]
				end
				return ret
			end)
		if not result then
			for i, moduledef in ipairs(part) do
				print(moduledef.source)
				error('circular dependency in a reflection batch')
			end
		end
		modulelist = result
	end

	local luaparts = {}
	loadluapartlist(luaparts, fenv.prolog, 'prolog')
	loadluapartlist(luaparts, fenv.epilog, 'epilog')

	do
		format.tofile(fenv.target,
			format.indexcpp, fenv, modulelist, luaparts)
	end
end

package.modtable(modname, rindex)
