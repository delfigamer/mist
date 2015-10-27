local modname = ...
local actorcomponent = require('game.actorcomponent')
local locationcomponent = actorcomponent:module(modname)
local location

function locationcomponent:init(parent)
	if parent then
		assert(parent['#game.locationcomponent'])
		self.parent = parent
	end
	self.location = location:create()
end

function locationcomponent:release()
end

function locationcomponent:setlocation(x, y, yaw)
	self.location.x = x
	self.location.y = y
	self.location.yaw = yaw
end

function locationcomponent:getworldlocation()
	if self.parent then
		local x, y, yaw = self.parent:getworldlocation()
		return x + self.location.x, y + self.location.y, yaw + self.location.yaw
	else
		return self.location.x, self.location.y, self.location.yaw
	end
end

location = require('game.location')
