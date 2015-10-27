local modname = ...
local object = require('base.object')
local page = object:module(modname)
local clearshape = require('host.clearshape')
local conf = require('config.index-editor')
local ffi = require('ffi')
local gifield = require('gi.gifield')
local info = require('host.info')
local input = require('system.input')
local resourceset = require('rs.resourceset')
local shapegroup = require('host.shapegroup')
local utility = require('base.utility')
local window = require('host.window')

local function assignhandler(page, cname, event, method)
	local ct = page.gifield.controls[cname]
	if ct then
		ct[event] = utility.method_s(page, method)
	end
end

function page:init()
	self.shapegroup = shapegroup:create()
	self.clearshape = clearshape:create()
	self.clearshape:setcolor(0, 0, 0, 1)
	self.shapegroup:insert(self.clearshape, -10)
	self.gifield = gifield:create(conf.gilayout)
	self.shapegroup:insert(self.gifield.shapegroup)
end

function page:register()
	input.handlers[2] = self.gifield
	input.handlers[3] = self
	window:setshape(self.shapegroup)
end

function page:unregister()
	input.handlers[3] = nil
	input.handlers[2] = nil
	self.inputhandler:unregister()
end

function page:handle(message, ...)
	local hf = self['on' .. message]
	if hf then
		hf(self, message, ...)
	end
end

function page:onpointmove(message, point, x, y)
	if point == 1 then
		local label = self.gifield.controls['testlabel']
		if label then
			label.position.width = x
			label.position.height = y
			label:updateposition()
		end
		local rect = self.gifield.controls['testrect']
		if rect then
			rect.position.width = x
			rect.position.height = y
			rect:updateposition()
		end
	end
end
