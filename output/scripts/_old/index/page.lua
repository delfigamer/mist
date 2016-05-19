local modname = ...
local object = require('base.object')
local page = object:module(modname)
local clearshape = require('host.clearshape')
local conf = require('config.index')
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
	assignhandler(self, 'button_1', 'onclick', self.button_1_click)
	assignhandler(self, 'button_2', 'onclick', self.button_2_click)
	assignhandler(self, 'button_3', 'onclick', self.button_3_click)
	assignhandler(self, 'button_4', 'onclick', self.button_4_click)
	assignhandler(self, 'button_5', 'onclick', self.button_5_click)
	assignhandler(self, 'button_exit', 'onclick', self.button_exit_click)
end

function page:register()
	input.handlers[2] = self.gifield
	window:setshape(self.shapegroup)
end

function page:unregister()
	input.handlers[2] = nil
	self.inputhandler:unregister()
end

function page:setlabel(text)
	local label = self.gifield.controls['label_test']
	if label then
		label:settext(text)
	end
end

function page:button_1_click(sender, point)
	if point == 1 then
		self:setlabel('WE')
	end
end

function page:button_2_click(sender, point)
	if point == 1 then
		self:setlabel('NEED A')
	end
end

function page:button_3_click(sender, point)
	if point == 1 then
		self:setlabel('DISPENSER')
	end
end

function page:button_4_click(sender, point)
	if point == 1 then
		self:setlabel('RIGHT')
	end
end

function page:button_5_click(sender, point)
	if point == 1 then
		self:setlabel('HERE')
	end
end

function page:button_exit_click(sender, point)
	if point == 1 then
		self:setlabel('POOTIS')
		window:finish()
	end
end
