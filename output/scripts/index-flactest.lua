require('utils.allocateconsole')()
local databuffer = require('host.databuffer')
local ffi = require('ffi')
local fileio = require('rs.fileio')
local flacptype = require('rs.flacptype')
local invoke = require('base.invoke')
local iostream = require('rs.iostream')
local rsutil = require('rs.util')

local function clampf(c)
	return (math.abs(c+1) - math.abs(c-1)) * 0.5
end

do
	local samplerate = 48000
	local soundlength = samplerate*5
	local frequency = 440
	local db = databuffer:create(soundlength*8, soundlength*8, nil)
	do
		local data = ffi.cast('struct{int32_t a,b;}*', db:getdata())
		for i = 0, soundlength-1 do
			local time = i/samplerate
			local phase = 2*math.pi*frequency*time
			local pha, phb = phase, phase
			local amp = (1 - math.exp(-500*time)) * math.exp(-1*time)
			local va, vb = amp*math.sin(pha), amp*math.sin(phb)
			data[i].a = clampf(va) * 0x7fffff
			data[i].b = clampf(vb) * 0x7fffff
			-- print(string.format('%#10x\t%#10x', data[i].a, data[i].b))
		end
	end
	local bm = {
		bitdepth = 24,
		channels = 2,
		samplerate = samplerate,
		db = db,
	}
	rsutil.saveptype(flacptype, bm, 'test.flac')
end

do
	local bm = rsutil.loadptype(flacptype, 'test.flac')
	log(bm.bitdepth, bm.channels, bm.samplerate)
	local samples = tonumber(bm.db:getlength()) / (4 * bm.channels)
	local data = ffi.cast('int32_t*', bm.db:getdata())
	log(samples)
	if samples > 32 then
		samples = 32
	end
	for i = 0, samples - 1 do
		local s = {}
		for ch = 0, bm.channels - 1 do
			s[ch + 1] = data[i * bm.channels + ch]
		end
		print(i, table.unpack(s))
	end
end
