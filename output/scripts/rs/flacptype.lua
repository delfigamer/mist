local modname = ...
local flacptype = package.modtable(modname)
local crc32 = require('crc32')
local databuffer = require('host.databuffer')
local ffi = require('ffi')
local flacreader = require('host.flacreader')
local flacwriter = require('host.flacwriter')
local memoryio = require('rs.memoryio')

flacptype.typename = modname
flacptype.typeid = crc32(modname)

function flacptype:write(bm, stream)
	local writer = flacwriter:create(
		bm.bitdepth, bm.channels, bm.samplerate, bm.db)
	while not writer:isfinished() do
		coroutine.yield()
	end
	local membuf = memoryio:create(writer:getbuffer())
	membuf:writetostream(stream)
	membuf:release()
	writer:release()
end

function flacptype:read(stream)
	local membuf = memoryio:create()
	membuf:readfromstream(stream)
	local reader = flacreader:create(membuf.hostio)
	while not reader:isfinished() do
		coroutine.yield()
	end
	local bm = {
		bitdepth = reader:getbitdepth(),
		channels = reader:getchannels(),
		samplerate = reader:getsamplerate(),
		db = reader:getdata(),
	}
	membuf:release()
	reader:release()
	return bm
end
