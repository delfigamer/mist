local modname = ...
local object = require('base.object')
local generator = object:module(modname)
local common = require('synth.common')
local func = require('synth.func')

function generator:init(it)
	self.phase = it.phase or 0
	self.freq = it.freq or 0
	self.freqscale = it.freqscale or 1
	self.amp = it.amp or 1
	self.offset = it.offset or 0
	self.filter = it.filter or func.identity
	self.wave = it.wave or func.sinwave
	self:reset()
end

function generator:reset()
	self.currentphase = 0
end

function generator:next()
	local value = self.wave(self.currentphase + self.phase)
	self.currentphase = math.fmod(
		self.currentphase + 2*math.pi*self.freq*self.freqscale*common.period,
		2*math.pi)
	return self.filter(value * self.amp + self.offset)
end
