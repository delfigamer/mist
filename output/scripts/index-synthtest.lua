local modname = ...
local synth = require('synth')
local object = require('base.object')

local instrument_a = object:derive(nil, modname .. ':instrument_a')

function instrument_a:reset(state)
	self.state = state
	self.wave_a = synth.func.wavegen()
	self.wave_b = synth.func.wavegen()
	self.amp_b = synth.func.lowpassgen(1000,1)
	self.freqgen = synth.func.lowpassgen(1000)
end

function instrument_a:nextentry(entry)
	self.amp_a = synth.func.lowpassgen(10, entry.gain)
end

function instrument_a:next(entry)
	local freq = self.freqgen(entry.freq)
	local amp = self.amp_b(self.amp_a(entry.gain*0.7))
	local v_a = self.wave_a(freq*2)
	local v_b = self.wave_b(freq, v_a*0.3)
	return v_b*amp
end

local instrument_b = object:derive(nil, modname .. ':instrument_b')

function instrument_b:reset(state)
	self.state = state
	self.wave_a = synth.func.wavegen()
	self.wave_b = synth.func.wavegen(synth.func.triangle)
	self.amp_b = synth.func.lowpassgen(1000,1)
	self.freqgen = synth.func.lowpassgen(1000)
end

function instrument_b:nextentry(entry)
	self.amp_a = synth.func.lowpassgen(2, entry.gain)
end

function instrument_b:next(entry)
	local freq = self.freqgen(entry.freq)
	local amp = self.amp_b(self.amp_a(entry.gain*0.2))
	local v_a = self.wave_a(freq/4)
	local v_b = self.wave_b(freq)
	return v_b*amp*(1+v_a*0.7)
end

local totallength, part_a, part_b, part_c, part_d, part_e = synth.note.parse{
	gamut = {0, 2, 3, 5, 7, 9, 10},
	str = [[
		ABCDE T160
		AB V-6 o4
		C V-6 o3
		DE V-6 o4

		ABCDE _2.

		A g4. a8 b4
		B d4. f8 g4
		C b4. _8 _4
		D _2.
		E _2.

		A a4 f4 g4
		B f4 c4 d4
		C _2.
		D _2.
		E _2.

		A -4 g8 a8 b4
		B -4 d8 -8 -4
		C _2.
		D _2.
		E _2.

		A a4 f4 g4
		B f4 c4 d4
		C _4 a4 _4
		D _2.
		E _2.

		A  g4. a8 b4
		B  e4. f8 g4
		C  b4. _8 _4
		D _2.
		E _2.

		A a4 g4  f4
		B f4 c4 <a4>
		C _2.
		D _2.
		E _2.

		A a8 b8 g2
		B d8 _8 d2
		C b8 -8 _2
		D _2 g8 a8
		E _2.

		A  _2     f4
		B  _2.
		C  _2.
		D >d4 c4< b4
		E  b4 a4  g4

		A g4. a8 b4
		B d4. f8 g4
		C b4. _8 _4
		D <g2.>
		E _2.

		A a4 f4 g4
		B f4 c4 d4
		C _2.
		D -2.
		E _2.

		A -4 g8 a8 b4
		B -4 d8 -8 -4
		C _2.
		D <g2.>
		E _2.

		A a4 f4 g4
		B f4 c4 d4
		C _4 a4 _4
		D <f2.>
		E _2.

		A g4. a8 b4
		B e4. f8 g4
		C b4. _8 _4
		D <e8 b8> e2
		E _2.

		A a4 g4 f4
		B f4 d4 c4
		C _2.
		D <f8> c8 f2
		E _2.

		A a8 b8 g2
		B d8 _8 d2
		C b8 -8 _2
		D _4      b4 _4
		E <g8> d8 g4 g8 a8

		A _4 g8 a8 b8 >c8<
		B _2.
		C _2.
		D f4 e4  d4
		E d4 c4 <b4>

		A >d4< g4. >d8<
		B  g4  e4.  _8
		C >e4< _4.  _8
		D  e2.
		E <e2.>

		A >c8< b8 a4 a4
		B  a8  _8 f4 f4
		C >f8< _8 _4 _4
		D _2.
		E <f4> c4 f4

		A a8 b8 g2
		B d8 _8 d2
		C _2.
		D _8   _8 b4 _8 _8
		E <g8> d8 g4 g8 a8

		A _4 g8 a8 b8 >c8<
		B _2.
		C _2.
		D f4 e4  d4
		E d4 c4 <b4>

		A >d4< g4. >d8<
		B  g4  e4.  _8
		C >e4< _4.  _8
		D  e2.
		E <e2.>

		A >c8< b8 a4 a4
		B  a8  _8 f4 f4
		C >f8< _8 _4 _4
		D _2.
		E <f4> c4 f4

		A b4 >d2<
		B f4  b2
		C _2.
		D _2.
		E <b4> f4 b4

		A _4 g8 a8 b8 >c8<
		B _2.
		C _2.
		D f4 e4  d4
		E d4 c4 <b4>

		A >d4< g4. >d8<
		B  g4  e4.  _8
		C >e4< _4.  _8
		D  e2.
		E <e2.>

		A >c4< b4 >c4<
		B  a4  g4  a4
		C >f4< _4  _4
		D _2.
		E <f8> c8 f8 g8 a4

		A >d8  e8 d2<
		B  b8  _8 _2
		C >g8< _8 _2
		D  g2.
		E <g2.>

		A >c8  d8 c2<
		B  a8  _8 _2
		C >f8< _8 _2
		D  f2.
		E <f2.>

		A  b8 >c8< b2
		B  g8  _8 _2
		C >e8< _8 _2
		D  e2.
		E <e2.>

		A  b8  a8 f4 f4
		B  f8  _8 _4 _4
		C >d8< _8 _4 _4
		D  d2.
		E <d2.>

		A a8 b8 g2
		B d8 _8 _2
		C b8 _8 _2
		D  <b2.>
		E <<b2.>>

		ABC _2.
		DE -2.
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
	synth.controller:create(instrument_a:create(), part_b),
	synth.controller:create(instrument_a:create(), part_c),
	synth.controller:create(instrument_b:create(), part_d),
	synth.controller:create(instrument_b:create(), part_e),
}

print(part_a.totallength / part_a.lengthscale)
print(part_b.totallength / part_a.lengthscale)
print(part_c.totallength / part_a.lengthscale)
print(part_d.totallength / part_a.lengthscale)
print(part_e.totallength / part_a.lengthscale)

local function filtergen(target)
	target:reset()
	return function()
		return math.tanh(target:next())
	end
end

synth.export{
	synth.wrapper:create(filtergen, mainmixer),
	path = 'test.flac',
	samples = synth.common.samplerate*totallength,
	lowbit = true,
}
