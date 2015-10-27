local modname = ...
local pngptype = package.modtable(modname)
local crc32 = require('crc32')
local databuffer = require('host.databuffer')
local ffi = require('ffi')
local pngreader = require('host.pngreader')
local pngwriter = require('host.pngwriter')

pngptype.typename = modname
pngptype.typeid = crc32(modname)

function pngptype:write(bm, stream)
	local writer = pngwriter:create(0, bm.width, bm.height, bm.db)
	local buffer = ffi.new('uint8_t[0x10000]')
	repeat
		local result = writer:grab(0x10000, buffer)
		stream:write(result, buffer)
	until writer:isfinished()
	writer:release()
end

function pngptype:read(stream)
	local reader = pngreader:create(0)
	local buffer = ffi.new('uint8_t[0x10000]')
	while true do
		local advance = stream:read(0x10000, buffer)
		reader:feed(advance, buffer)
		if advance < 0x10000 then
			if reader:isfinished() then
				break
			else
				error('invalid stream')
			end
		end
	end
	local result = {
		width = reader:getwidth(),
		height = reader:getheight(),
		db = reader:getdata(),
	}
	reader:release()
	return result
end
