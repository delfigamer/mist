local modname = ...
local object = require('base.object')
local spritefield = object:module(modname)
local meshshape = require('host.meshshape')
local spritemeshdata = require('graphics.spritemeshdata')
local sprite = require('graphics.sprite')

function spritefield:init()
	self.meshdata = spritemeshdata:create()
	self.msresource = meshshape:create()
	self.msresource:setmeshdata(self.meshdata:getmdresource())
end

function spritefield:getmsresource()
	return self.msresource
end

function spritefield:createsprite()
	local sp = sprite:create(self)
	self.meshdata:insertsprite(sp)
	return sp
end

function spritefield:removesprite(sp)
	self.meshdata:removesprite(sp)
end

function spritefield:update()
	self.meshdata:update()
end
