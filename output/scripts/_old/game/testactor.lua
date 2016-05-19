local modname = ...
local actor = require('game.actor')
local testactor = actor:module(modname)
local tickcomponent
local locationcomponent

function testactor:init()
	actor.init(self)
	self.tickcomponent = self:createcomponent(tickcomponent)
	self.root = self:createcomponent(locationcomponent)
end

function testactor:release()
	actor.release(self)
end

function testactor:tick(tframe)
	print(tframe/50)
end

tickcomponent = require('game.tickcomponent')
locationcomponent = require('game.locationcomponent')
