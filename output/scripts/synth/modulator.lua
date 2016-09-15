local modname = ...
local object = require('base.object')
local modulator = object:module(modname)
local common = require('synth.common')
local func = require('synth.func')

function modulator:init(it)
	self.source = assert(it.source)
	self.target = assert(it.target)
	self.mode = it.mode or 'amp'
	self:reset()
end

function modulator:reset()
	self.source:reset()
	self.target:reset()
end

function modulator:next()
	local arg = self.source:next()
	self.target[self.mode] = arg
	return self.target:next()
end
