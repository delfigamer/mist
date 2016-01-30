local modname = ...
local object = require('base.object')
local page = object:module(modname)
local ffi = require('ffi')
local info = require('host.info')
local input = require('system.input')
local types = require('host.types')
local window = require('host.window')

local clearshape = require('host.clearshape')
local shapegroup = require('host.shapegroup')
local flarefield = require('index-test.field')

function page:init()
	self.shape = shapegroup:create()
	self.clearshape = clearshape:create()
	self.clearshape:setcolor(0, 0, 0, 1)
	self.clearshapeiterator = self.shape:insert(self.clearshape, -100)
	self.flarefield = flarefield:create()
	self.meshshapeiterator =
		self.shape:insert(self.flarefield:getshaperesource())
end

function page:register()
	input.handlers[2] = self
	window:setshape(self.shape)
end

function page:unregister()
	window:setshape()
	input.handlers[2] = nil
end

function page:handle(message, ...)
	local hf = self['on'..message]
	if hf then
		hf(self, message, ...)
	end
end

function page:onpointmove(msg, point, x, y)
	self.flarefield:settarget(x - info.width/2, info.height/2 - y)
end

function page:onpointdown(msg, point, x, y)
	if point == 1 then
		self.flarefield:createflare(x - info.width/2, info.height/2 - y)
	end
end

function page:onpointup(msg, point)
end

function page:ontick(msg, dt)
	self.flarefield:tick(dt)
end
