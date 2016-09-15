local modname = ...
local common = require('synth.common')
local databuffer = require('host.databuffer')
local ffi = require('ffi')
local flacptype = require('rs.flacptype')
local func = require('synth.func')
local rsutil = require('rs.util')

local function export(it)
	local rate = common.samplerate
	local samples = it.samples
	local channels = #it
	local len = samples * channels * 4
	local bm = {
		bitdepth = it.lowbit and 16 or 24,
		channels = channels,
		samplerate = rate,
		db = databuffer:create(len, len),
	}
	local scale = it.lowbit and 0x7fff or 0x7fffff
	local ptr = ffi.cast('int32_t*', bm.db:getdata())
	for channel = 0, channels-1 do
		it[channel + 1]:reset()
	end
	for si = 0, samples-1 do
		sptr = ptr + si * channels
		for channel = 0, channels-1 do
			local subsample = it[channel + 1]:next()
			sptr[channel] = math.floor(func.clamp(subsample) * scale + 0.5)
			for cycle = 1, common.subcycles-1 do
				it[channel + 1]:next()
			end
		end
	end
	rsutil.saveptype(flacptype, bm, it.path)
end

package.modtable(modname, export)
