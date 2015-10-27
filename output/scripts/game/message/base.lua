local modname = ...
local ffipure = require('base.ffipure')
local message = ffipure:module(modname)

message.messagetable = {}

function message:derive(target, modname, id, name)
	local child = ffipure.derive(self, target, modname)
	if id then
		child.messageid = id
		child.messagename = name
		message.messagetable[id] = child
		message.messagetable[name] = child
	end
	return child
end

function message:create(target, ...)
	return self:new(self.messageid, target, ...)
end

message.fields = 'int event; int target;'
