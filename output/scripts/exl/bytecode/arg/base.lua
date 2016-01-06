local modname = ...
local object = package.relrequire(modname, 3, 'object')
local basearg = object:module(modname)
local crc32 = require('crc32')
local scalars = require('rs.scalars')

local argclasses = {}

function basearg:derive(target, modname)
	local class = object.derive(self, target, modname)
	class.nameid = crc32(modname)
	argclasses[class.nameid] = class
	return class
end

function basearg:createarg(it)
	return argclasses[it[1]]:create(it)
end

function basearg:writearg(stream)
	scalars.int32:write(self.nameid, stream)
	self:write(stream)
end

function basearg:readarg(stream)
	local nameid = scalars.int32:read(stream)
	local class = argclasses[nameid]
	if not class then
		error('invalid stream')
	end
	return class:read(stream)
end

for i, name in ipairs{
		'block',
		'int',
		'list',
		'local',
		'member',
		'number',
		'ssa',
		'string',
	} do
	argclasses[name] = package.relrequire(modname, 1, name)
end
