local modname = ...
local locationcomponent = require('game.locationcomponent')
local spritecomponent = locationcomponent:module(modname)
local types

function spritecomponent:init(parent)
	locationcomponent.init(self, parent)
	self.subsystem = self.actor.world:requiresubsystem('game.displaysubsystem')
	self.sprite = self.subsystem.spritefield:newsprite()
	self.width = 20
	self.height = 10
	self.sprite:setcolor(types.colortype(1, 1, 1, 1))
end

function spritecomponent:release()
	self.sprite:release()
	locationcomponent.release(self)
end

function spritecomponent:updateposition()
	local x, y, yaw = self:getworldlocation()
	self.sprite:setposition(x, y)
	self.sprite:setextent(self.width*math.cos(yaw), self.width*math.sin(yaw),
		-self.height*math.sin(yaw), self.height*math.cos(yaw))
end

function spritecomponent:event_setlocation(message)
	self:updateposition()
end

types = require('host.types')
