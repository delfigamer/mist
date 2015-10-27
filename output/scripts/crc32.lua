local modname = ...
local ffi = require('ffi')

local polynomial = 0x82F63B78 -- CRC-32C
local lookup = ffi.new('uint32_t[256]')
for i = 0, 255 do
	local crc = i
	for j = 0, 7 do
		crc = bit.bxor(bit.rshift(crc, 1), bit.band(-bit.band(crc, 1), polynomial))
	end
	lookup[i] = crc
end

local function crc32(data, length, start)
	local ptr = ffi.cast('uint8_t const*', data)
	if type(data) == 'string' and not length then
		length = #data
	end
	local crc = bit.bnot(start or 0)
	for i = 0, length-1 do
		crc = bit.bxor(bit.rshift(crc, 8), lookup[bit.bxor(bit.band(crc, 255), ptr[i])]);
	end
	return bit.bnot(crc)
end

return crc32
