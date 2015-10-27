local modname = ...
local windowbase = require('gi.windowbase')
local gifield = windowbase:module(modname)
local bit = require('bit')
local control = require('gi.control')
local info = require('host.info')
local multisf = require('gi.multispritefield')
local shapegroup = require('host.shapegroup')
local textfieldset = require('gi.textfieldset')

function gifield:init(it)
	windowbase.init(self, it)
	self.gifield = self
	self.shapegroup = shapegroup:create()
	self.multisf = multisf:create(self.shapegroup, 0)
	self.textmultisf = multisf:create(self.shapegroup, 10)
	self.textfieldset = textfieldset:create()
	self.controls = {}
	for i, cit in ipairs(it) do
		control:createcontrol(cit, self)
	end
end

function gifield:handle_pointmove(message, point, x, y)
	windowbase.handle_pointmove(self, message, point, x, info.height - 1 - y)
end

function gifield:handle_pointdown(message, point, x, y)
	windowbase.handle_pointdown(self, message, point, x, info.height - 1 - y)
	windowbase.handle_pointenter(self, message, point, x, info.height - 1 - y)
end

function gifield:handle_pointup(message, point)
	windowbase.handle_pointleave(self, message, point)
	windowbase.handle_pointup(self, message, point)
end
