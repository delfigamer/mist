local modname = ...
local object = require('base.object')
local controller = object:module(modname)
local common = require('synth.common')

function controller:init(target, ct)
	self.target = target
	self.ct = ct
	self:reset()
end

function controller:reset()
	if self.target.reset then
		self.target:reset(state)
	end
	self.index = 1
	self.entry = self.ct[1]
	self.time = 0
	if self.entry and self.target.nextentry then
		self.target:nextentry(self.entry)
	end
end

function controller:next()
	while self.index < #self.ct and self.time >= self.entry[1] do
		self.time = self.time - self.entry[1]
		self.index = self.index + 1
		self.entry = self.ct[self.index]
		if self.target.nextentry then
			self.target:nextentry(self.entry)
		end
	end
	local ret = self.target:next(self.entry)
	self.time = self.time + common.period
	return ret
end
