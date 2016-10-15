local modname = ...
local listptype = package.modtable(modname)
local crc32 = require('crc32')
local scalars = require('rs.scalars')

listptype.typename = 'list'
listptype.typeid = crc32(listptype.typename)

local function writeitem(item, stream)
	local itype = type(item)
	if itype == 'table' then
		scalars.uint32:write(listptype.typeid, stream)
		listptype:write(item, stream)
	elseif itype == 'number' then
		scalars.uint32:write(scalars.double.typeid, stream)
		scalars.double:write(item, stream)
	elseif itype == 'string' then
		scalars.uint32:write(scalars.string.typeid, stream)
		scalars.string:write(item, stream)
	elseif itype == 'boolean' then
		scalars.uint32:write(scalars.int32.typeid, stream)
		scalars.string:write(item and 1 or 0, stream)
	else
		error('invalid element type')
	end
end

local function readitem(stream)
	local typeid = scalars.uint32:read(stream)
	if typeid == listptype.typeid then
		return listptype:read(stream)
	elseif typeid == scalars.double.typeid then
		return scalars.double:read(stream)
	elseif typeid == scalars.string.typeid then
		return scalars.string:read(stream)
	elseif typeid == scalars.int32.typeid then
		return scalars.string:read(stream) ~= 0
	else
		error('invalid element type')
	end
end

function listptype:write(instance, stream)
	scalars.int:write(#instance, stream)
	for i = 1, #instance do
		writeitem(instance[i], stream)
	end
end

function listptype:read(stream)
	local length = scalars.int:read(stream)
	local instance = {}
	for i = 1, length do
		instance[i] = readitem(stream)
	end
	return instance
end
