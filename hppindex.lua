assert(loadfile('base.lua'))()
setfenv(1, _G)
protectglobaltable(true)
local env = require('env')
local format = require('hppformat')

local filelist = {...}
local targetpath = _G.targetpath or error('targetpath must be set')
local indexname = _G.indexname or error('indexname must be set')
local prolog = _G.prolog or ''
local epilog = _G.epilog or ''

local modulelist = {}
local modulemap = {}

local function createrenv(moduledef)
	return {
		modulename = function(name)
			moduledef.name = name
			moduledef.source = name .. '.hpp'
			moduledef.identname = string.gsub(name, '[^a-zA-Z0-9_]', '_')
			modulemap[name] = moduledef
		end,
		load = function(targetname)
			table.append(moduledef.includes, targetname)
		end,
		nslookup = function() end,
		decl = function() end,
		registerdecl = function() end}
end

local function loadmodule(filename)
	local moduledef = {
		includes = {}}
	local file = assert(io.open(env.path(filename), 'r'))
	local source = file:read('*a')
	file:close()
	assert(load(source, rpath, 't', createrenv(moduledef)))()
	return moduledef
end

for i, filename in ipairs(filelist) do
	local moduledef = loadmodule(filename)
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
for source in string.gmatch(prolog, '[^;]+') do
	local part = {
		prolog = true,
		chunk = string.dump(assert(loadfile(env.path(source)))),
		name = source}
	table.append(luaparts, part)
end
for source in string.gmatch(epilog, '[^;]+') do
	local part = {
		epilog = true,
		chunk = string.dump(assert(loadfile(env.path(source)))),
		name = source}
	table.append(luaparts, part)
end

local indexnameparts = {}
for part in string.gmatch(indexname, '[^:]+') do
	table.append(indexnameparts, part)
end

local fenv = {
	targetpath = targetpath,
	indexnameparts = indexnameparts,
	luaparts = luaparts,
}

do
	format.tofile(targetpath,
		format.indexcpp, fenv, modulelist)
end
