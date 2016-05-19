local modname = ...
local actorcomponent = require('game.actorcomponent')
local tickcomponent = actorcomponent:module(modname)

function tickcomponent:init()
	self.subsystem = self.actor.world:requiresubsystem('game.ticksubsystem')
	self.subsystem:register(self)
	self.counter = 0
end

function tickcomponent:release()
	self.subsystem:unregister(self)
end

function tickcomponent:tick()
	self.counter = self.counter + 1
	self.actor:tick(self.counter)
end
