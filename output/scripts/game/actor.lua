local modname = ...
local object = require('base.object')
local actor = object:module(modname)
local set

local function component_remove(component)
	component:release()
end

function actor:create()
	error('use world:spawn()')
end

function actor:init()
	self.components = set:create()
	self.components.onremove = component_remove
end

function actor:release()
	self.components:clear()
end

function actor:createcomponent(class, ...)
	local component = class:new()
	component.actor = self
	self.components:insert(component)
	component:init(...)
	return component
end

function actor:releasecomponent(component)
	self.components:remove(component)
end

function actor:receivemessage(message)
	local ename = message.messagename
	local handler = self['event_' .. ename]
	if handler then
		handler(self, message)
	end
	for i, component in self.components:pairs() do
		local handler = component['event_' .. ename]
		if handler then
			handler(component, message)
		end
	end
end

function actor:event_echo(message)
	print(self:defstring())
end

function actor:event_kill(message)
	self.world:markactorforkill(self)
	self.isdead = true
end

set = require('base.set')
