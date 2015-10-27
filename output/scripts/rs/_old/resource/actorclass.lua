local modname = ...
local base = require('rs.resource.base')
local actorclass = base:module(modname, 'ActorClass')
local exttable = require('rs.resource.exttable')
local luaformat = require('rs.format.lua')

exttable['actor'] = actorclass

actorclass.isluaresource = true
actorclass.format = luaformat
actorclass.searchpath = '?.actor'

function actorclass:createnew()
	error('cannot create script resources')
end

function actorclass:afterloading(task)
end

function actorclass:getsource()
	self:finishloading()
	return self.source
end

function actorclass:getclass()
	self:finishloading()
	return self.class
end
