local modname = ...
local dbptype = package.modtable(modname)
local databuffer = require('host.databuffer')
local crc32 = require('crc32')
local dword = require('host.types').dword
local scalars = require('rs.scalars')

dbptype.typename = 'host.databuffer'
dbptype.typeid = crc32(dbptype.typename)

function dbptype:write(db, stream)
	local length = db:getlength()
	scalars.uint32:write(length, stream)
	scalars.uint32:write(db:getcapacity(), stream)
	if stream:write(length, db:getdata()) ~= length then
		error('i/o failure')
	end
end

function dbptype:read(stream)
	local length = scalars.uint32:read(stream)
	local capacity = scalars.uint32:read(stream)
	if capacity < length then
		error('invalid databuffer field')
	end
	local db = databuffer:create(length, capacity)
	if stream:read(length, db:getdata()) ~= length then
		error('i/o failure')
	end
	return db
end
