local ffi = require('ffi')
local fileio = require('rs.fileio')
local invoke = require('base.invoke')
local memoryio = require('rs.memoryio')

local function copyio(source, target)
	local buffer = ffi.new('uint8_t[0x10000]')
	local offset = 0ULL
	while true do
		local result = source:ioread(offset, 0x10000, buffer)
		target:iowrite(offset, result, buffer)
		offset = offset + result
		if result == 0 then
			break
		end
	end
	target:setiosize(offset)
	return offset
end

local source = fileio:create('a.txt', 'r')
local membuf = memoryio:create()
local result = invoke(copyio, source, membuf)
log(string.format('transferred %s bytes from source to memory', result))
local target = fileio:create('b.txt', 'p+')
result = invoke(copyio, membuf, target)
log(string.format('transferred %s bytes from memory to target', result))
