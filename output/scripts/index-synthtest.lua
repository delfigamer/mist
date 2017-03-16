local modname = ...
local synth = require('synth')
local object = require('base.object')
local func = synth.func

local instrument_a = object:derive(nil, modname .. ':instrument_a')

function instrument_a:reset(state)
	self.state = state
	self.wave_a = func.wavegen()
	self.wave_b = func.wavegen()
	self.amp_b = func.inertialnode(0.001,0)
	self.filt1 = func.resonantfilter(100, 0.001)
end

function instrument_a:nextentry(entry)
	self.amp_a = func.inertialnode(0.2, entry.gain)
end

function instrument_a:next(entry)
	-- local amp = self.amp_b(self.amp_a(0))
	local v_a = math.random() - math.random()
	local v_b = self.filt1(v_a*1000)
	return v_b
end

local totallength, part_a, part_b, part_c, part_d, part_e = synth.note.parse{
	gamut = {0, 2, 3, 5, 7, 9, 10},
	str = [[
		ABCDE T160
		AB V-6 o4
		C V-6 o3
		DE V-6 o4

		A c1
		A _1
		A c32
	]],
}

--  -c   6
--   b
--  -a
--   g
-----f--
--   e
-----d--
--   c   5
-----b--
--   a
-----g--
--   f
-----e--
--   d
--  -c   4
--   b
-----a--
--   g
-----f--
--   e
-----d--
--   c   3
-----b--
--   a
-----g--
--   f
--  -e
--   d
--  -c   2

local mainmixer = synth.mixer:create{
	synth.controller:create(instrument_a:create(), part_a),
}

print(part_a.totallength / part_a.lengthscale)
-- print(part_b.totallength / part_a.lengthscale)
-- print(part_c.totallength / part_a.lengthscale)
-- print(part_d.totallength / part_a.lengthscale)
-- print(part_e.totallength / part_a.lengthscale)

local function filtergen(target)
	target:reset()
	return function()
		return math.tanh(target:next())
	end
end

synth.export{
	-- synth.wrapper:create(filtergen, mainmixer),
	instrument_a:create(),
	path = 'test.flac',
	samples = synth.common.samplerate*60,
	lowbit = true,
}
