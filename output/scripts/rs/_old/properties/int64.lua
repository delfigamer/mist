local modname = ...
local int64 = package.modtable(modname)
local crc32 = require('crc32')
local dword = require('host.types').dword
local ffi = require('ffi')
local int = require('rs.properties.int')

function int64.write(i, stream)
	local u = dword(i)
	int.write(u.h, stream)
	int.write(u.l, stream)
end

function int64.read(stream)
	local u = dword()
	u.h = int.write(stream)
	u.l = int.write(stream)
	return u.d
end

int64.typeid = crc32('int64')
