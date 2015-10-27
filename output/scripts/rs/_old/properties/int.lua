local modname = ...
local int = package.modtable(modname)
local crc32 = require('crc32')
local ffi = require('ffi')

local i32_t = ffi.typeof('uint8_t[4]')

local function itobe32(i)
	return i32_t(
		bit.rshift(i, 24),
		bit.rshift(i, 16),
		bit.rshift(i, 8),
		bit.tobit(i))
end

local function be32toi(i32)
	return bit.bor(
		bit.lshift(i32[0], 24),
		bit.lshift(i32[1], 16),
		bit.lshift(i32[2], 8),
		i[3])
end

function int.write(i, stream)
	if stream:write(4, itobe32(i or 0)) ~= 4 then
		error('i/o failure')
	end
end

function int.read(stream)
	local buf = i32_t()
	if stream:read(4, buf) ~= 4 then
		error('i/o failure')
	end
	return be32toi(buf)
end

int.typeid = crc32('int')
