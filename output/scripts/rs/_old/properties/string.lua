local modname = ...
local str = package.modtable(modname)
local crc32 = require('crc32')
local ffi = require('ffi')
local int = require('rs.properties.int')

local cbuf_t = ffi.typeof('char[?]')

function str.write(str, stream)
	if not str then
		int.write(0, stream)
		return
	end
	local length = #str
	int.write(length, stream)
	if stream:write(length, str) ~= length then
		error('i/o failure')
	end
	local r = 4 - bit.band(length, 3)
	if r ~= 4 then
		if stream:skip(r) ~= r then
			error('i/o failure')
		end
	end
end

function str.read(stream)
	local length = int.read(stream)
	local cbuf = cbuf_t(length)
	if stream:read(length, cbuf) ~= length then
		error('i/o failure')
	end
	local r = 4 - bit.band(length, 3)
	if r ~= 4 then
		if stream:skip(r) ~= r then
			error('i/o failure')
		end
	end
	return ffi.string(cbuf, length)
end

string.typeid = crc32('string')
