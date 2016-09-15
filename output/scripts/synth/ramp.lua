local modname = ...
local object = require('base.object')
local ramp = object:module(modname)
local common = require('synth.common')
local func = require('synth.func')

function ramp:init(it)
	self.base = it.base or 0
	self.rate = it.rate or 0
	self.max = it.max or 0
	self.min = it.min or 0
	self:reset()
end

function ramp:reset()
	self.value = self.base
end

function ramp:next()
	local value = self.value
	local v = self.value + self.rate*common.period
	self.value = (self.min + self.max +
		math.abs(v-self.min) - math.abs(v-self.max)) * 0.5
	return value
end
