local modname = ...
local rcache = package.modtable(modname)
local env = require('env')
local parser = require('reflect-parser')
local rtypes = require('reflect-types')

local modulenamemap = {}
local modulepathmap = {}
local defaultnamespace = {}

for i, name in ipairs{
	'int8_t',
	'int16_t',
	'int32_t',
	'int64_t',
	'uint8_t',
	'uint16_t',
	'uint32_t',
	'uint64_t',
	'size_t',
	'ptrdiff_t',
	'intptr_t',
	'uintptr_t'}
do
	defaultnamespace['::' .. name] = {
		id = 'default ::' .. name,
		scalartype = true,
		sourcename = '::' .. name,
		luaname = name,
		commonname = name,
		location = {{'default', 1, 1}}}
end

function rcache.registermodule(name, source)
	local moduledef = {
		name = name,
		source = source,
		includes = {},
		decls = {},
		namespace = {}
	}
	moduledef.identname = string.gsub(moduledef.name, '[^a-zA-Z0-9_]', '_')
	local path = 'reflection/' .. name .. '.r',
	assert(modulenamemap[moduledef.name] == nil)
	assert(modulenamemap[path] == nil)
	modulenamemap[moduledef.name] = moduledef
	modulepathmap[path] = moduledef
	for name, decl in pairs(defaultnamespace) do
		moduledef.namespace[name] = decl
	end
	return moduledef
end

function rcache.registerdecl(moduledef, decl)
	if
		decl.rstruct or
		decl.rclass or
		decl.rexternal or
		decl.renum
	then
		local sfname = decl.sourcename
		local prev = moduledef.namespace[sfname]
		if prev then
			if decl.rexternal and (prev.rclass or prev.rexternal) then
				return
			elseif decl.rclass and prev.rexternal then
				moduledef.namespace[sfname] = decl
			else
				parser.locationerror(decl.location,
					sfname .. ' has already been declared' ..
					parser.locationstring(prev.location))
			end
		else
			moduledef.namespace[sfname] = decl
		end
	end
end

function rcache.includemodule(moduledef, target)
	table.append(moduledef.includes, target.name)
	for name, decl in pairs(target.namespace) do
		local prev = moduledef.namespace[name]
		if not prev then
			moduledef.namespace[name] = decl
		elseif decl.id == prev.id then
			-- nothing
		elseif decl.rexternal and (prev.rclass or prev.rexternal) then
			-- nothing
		elseif decl.rclass and prev.rexternal then
			moduledef.namespace[name] = decl
		else
			error('failed to include ' .. target.name .. ' into ' ..
				moduledef.name .. ': conflicting declarations for ' .. name)
		end
	end
end

local loadmodule_env = {
	types = rtypes}

function loadmodule_env.registermodule(name, source)
	return rcache.registermodule(name, source)
end

function loadmodule_env.include(moduledef, targetname)
	local target = assert(rcache.tryloadmodulebyname(targetname))
	rcache.includemodule(moduledef, target)
end

function loadmodule_env.nslookup(moduledef, sourcename)
	local decl = moduledef.namespace[sourcename]
	if not decl then
		error('unknown name ' .. sourcename)
	end
	return decl
end

function loadmodule_env.decl(moduledef, decl)
	table.append(moduledef.decls, decl)
	rcache.registerdecl(moduledef, decl)
end

function rcache.tryloadmodulebyname(name)
	local moduledef = modulenamemap[name]
	if not moduledef then
		local path = 'reflection/' .. name .. '.r'
		return rcache.tryloadmodulebypath(path)
	end
	return moduledef
end

function rcache.tryloadmodulebypath(path)
	local moduledef = modulepathmap[path]
	if not moduledef then
		local file = io.open(env.path(path), 'r')
		if not file then
			return nil
		end
		local source = file:read('*a')
		file:close()
		assert(load(source, path, 't', loadmodule_env))()
		moduledef = assert(modulepathmap[path])
	end
	return moduledef
end
