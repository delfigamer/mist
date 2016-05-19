local modname = ...
local object = require('base.object')
local world = object:module(modname)
local message

function world:init()
	self.actors = {}
	self.nextid = 1
	self.subsystems = {}
	self.actorsforkill = {}
end

function world:spawn(class, ...)
	local actor = class:new()
	actor.world = self
	actor.id = self.nextid
	self.nextid = self.nextid + 1
	self.actors[actor.id] = actor
	actor:init()
	return actor
end

function world:dispatch(msg)
	local actor = self.actors[msg.target]
	if actor then
		local suc, err = pcall(actor.receivemessage, actor, msg)
		if not suc then
			print(err)
		end
	end
end

function world:send(mname, target, ...)
	local messageclass = message.messagetable[mname]
	if messageclass then
		local msg = messageclass:create(target, ...)
		self:dispatch(msg)
	else
		error('unknown mesage: ' .. mname)
	end
end

function world:requiresubsystem(name)
	local subs = self.subsystems[name]
	if not subs then
		subs = require(name):create(self)
		self.subsystems[name] = subs
	end
	return subs
end

function world:markactorforkill(actor)
	self.actorsforkill[#self.actorsforkill + 1] = actor
end

function world:releasekilledactors()
	for i, actor in ipairs(self.actorsforkill) do
		self.actors[actor.id] = nil
		actor:release()
		self.actorsforkill[i] = nil
	end
end

message = require('game.message.base')
require('game.message')
