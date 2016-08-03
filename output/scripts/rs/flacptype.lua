local modname = ...
local flacptype = package.modtable(modname)
local crc32 = require('crc32')
local databuffer = require('host.databuffer')
local ffi = require('ffi')
-- local flacreader = require('host.flacreader')
local flacwriter = require('host.flacwriter')

flacptype.typename = modname
flacptype.typeid = crc32(modname)

function flacptype:write(bm, stream)
	local writer = flacwriter:create(
		bm.bitdepth, bm.channels, bm.samplerate, bm.db)
	local buffer = ffi.new('uint8_t[0x10000]')
	repeat
		local result = writer:grab(0x10000, buffer)
		stream:write(result, buffer)
	until writer:isfinished()
	writer:release()
end

function flacptype:read(stream)
	-- local reader = pngreader:create(0)
	-- local buffer = ffi.new('uint8_t[0x10000]')
	-- while true do
		-- local advance = stream:read(0x10000, buffer)
		-- reader:feed(advance, buffer)
		-- if advance < 0x10000 then
			-- if reader:isfinished() then
				-- break
			-- else
				-- error('invalid stream')
			-- end
		-- end
	-- end
	-- local result = {
		-- width = reader:getwidth(),
		-- height = reader:getheight(),
		-- db = reader:getdata(),
	-- }
	-- reader:release()
	-- return result
	return {}
end
