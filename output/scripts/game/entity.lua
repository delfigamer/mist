local modname = ...
local actor = require('game.actor')
local entity = actor:module(modname)
local tickcomponent
local locationcomponent
local spritecomponent

function entity:init()
	actor.init(self)
	self.root = self:createcomponent(locationcomponent)
	self.tickcomponent = self:createcomponent(tickcomponent)
	self.sprite = self:createcomponent(spritecomponent, self.root)
end

function entity:release()
	actor.release(self)
end

function entity:tick(tframe)
	local phase = tframe*math.pi/100
	self:setlocation(math.cos(phase)*100, math.sin(phase)*100, phase)
end

function entity:setlocation(x, y, yaw)
	self.world:send('setlocation', self.id, x, y, yaw)
end

function entity:event_setlocation(message)
	if self.root and self.root['#game.locationcomponent'] then
		self.root:setlocation(message.x, message.y, message.yaw)
	end
end

tickcomponent = require('game.tickcomponent')
locationcomponent = require('game.locationcomponent')
spritecomponent = require('game.spritecomponent')
