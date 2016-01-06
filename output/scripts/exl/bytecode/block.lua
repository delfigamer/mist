local modname = ...
local object = package.relrequire(modname, 2, 'object')
local block = object:module(modname)
local crc32 = require('crc32')
local scalars = require('rs.scalars')
local token = package.relrequire(modname, 1, 'token')

block.ptype = {}

block.ptype.typename = 'exl.bytecode'
block.ptype.typeid = crc32(block.ptype.typename)

function block.ptype:write(instance, stream)
	scalars.int:write(#instance.parts, stream)
	for i = 1, #instance.parts do
		instance.parts[i]:write(stream)
	end
end

function block.ptype:read(stream)
	local length = scalars.int:read(stream)
	local instance = block:new()
	instance.parts = {}
	for i = 1, length do
		instance.parts[i] = token:read(stream)
	end
	return instance
end

function block:init()
	self.parts = {}
	self.lastname = 0
end

function block:write(stream)
	self.ptype:write(self, stream)
end

function block:read(stream)
	self.ptype:read(stream)
end

function block:genname()
	self.lastname = self.lastname + 1
	return self.lastname
end

function block:writetoken(it)
	local t = token:create(it)
	table.append(self.parts, t)
end

function block:defstring(lp)
	local lines = {}
	for i, part in ipairs(self.parts) do
		lines[i] = lp .. part:defstring(lp) .. '\n'
	end
	return table.concat(lines)
end
