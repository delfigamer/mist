-- See filecomparer.lua for difference map layout.

require('args')
crc32 = require('crc32')
bit = require('bit')

local sourcepath, targetpath, diffpath = args.files[1], args.files[2], args.files[3]

if not sourcepath or not targetpath or not diffpath then
	print('syntax:')
	print('diffapplier.lua <source> <target> <output>')
	return
end

local function decodeint(str, pos)
	pos = pos or 1
	return bit.bor(
		bit.lshift(string.byte(str, pos), 24),
		bit.lshift(string.byte(str, pos+1), 16),
		bit.lshift(string.byte(str, pos+2), 8),
		string.byte(str, pos+3))
end

local sourcefile = assert(io.open(sourcepath, 'rb'))
local targetfile = assert(io.open(targetpath, 'wb'))
local difffile = assert(io.open(diffpath, 'rb'))
while true do
	local header = difffile:read(16)
	if not header then
		break
	end
	local action = decodeint(header, 1)
	local sourcelen = decodeint(header, 5)
	local source_crc = decodeint(header, 9)
	local targetlen = decodeint(header, 13)
	if action == 0 then
		break
	elseif action == 1 then
		assert(targetlen == 0, 'malformed diff map')
		local data = sourcefile:read(sourcelen)
		assert(crc32(data) == source_crc, 'source data checksum mismatch')
		targetfile:write(data)
	elseif action == 2 then
		local data = sourcefile:read(sourcelen)
		assert(crc32(data) == source_crc, 'source data checksum mismatch')
		data = difffile:read(targetlen)
		difffile:seek('cur', 15 - (targetlen + 15) % 16)
		targetfile:write(data)
	else
		error('malformed diff map')
	end
end
