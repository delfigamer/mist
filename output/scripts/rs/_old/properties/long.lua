local modname = ...
local long = package.modtable(modname)
local crc32 = require('crc32')
local dword = require('host.types').dword
local ffi = require('ffi')
local int = require('rs.properties.int')

function long.write(i, stream)
	local u = dword(i)
	if u.h == 0 and u.l <= 0x7fffffff then
		int.write(u.l, stream)
	elseif u.h <= 0x7fffffff then
		int.write(bit.bor(u.h, 0x80000000), stream)
		int.write(u.l, stream)
	else
		error('number is too large')
	end
end

function long.read(stream)
	local lead = int.write(stream)
	if bit.bor(lead, 0x80000000) == 0 then
		return lead
	else
		local u = dword()
		u.h = bit.band(lead, 0x7fffffff)
		u.l = int.read(stream)
		return u.d
	end
end

long.typeid = crc32('long')
