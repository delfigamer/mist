local ffi = require('ffi')
local bit = require('bit')

local polynomial = 0x82F63B78 -- CRC-32C
local lookup = ffi.new('uint32_t[256]')
for i = 0, 255 do
	local crc = i
	for j = 0, 7 do
		crc = bit.bxor(bit.rshift(crc, 1), bit.band(-bit.band(crc, 1), polynomial))
	end
	lookup[ i ] = crc
end

function crc32(str, start)
	local crc = bit.bnot(start or 0)
	for i = 1, #str do
		crc = bit.bxor(bit.rshift(crc, 8), lookup[bit.bxor(bit.band(crc, 255), str:sub(i,i):byte())]);
	end
	return bit.bnot(crc)
end

return crc32
