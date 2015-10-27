local modname = ...
local object = require('base.object')
local ticksubsystem = object:module(modname)
local set
local utility

function ticksubsystem:init(world)
	self.components = set:create()
	self.timediff = 0
	self.period = 0.02
end

function ticksubsystem:register(component)
	self.components:insert(component)
end

function ticksubsystem:unregister(component)
	self.components:remove(component)
end

function ticksubsystem:tick()
	for i, comp in self.components:pairs() do
		comp:tick()
	end
end

function ticksubsystem:advance(dt)
	self.timediff = self.timediff + dt
	if self.timediff > self.period then
		self:tick()
		self.timediff = self.timediff - self.period
		if self.timediff > self.period then
			self.timediff = 0
		end
	end
end

set = require('base.set')
utility = require('base.utility')
