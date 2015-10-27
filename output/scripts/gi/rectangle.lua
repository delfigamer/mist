local modname = ...
local control = require('gi.control')
local rectangle = control:module(modname)
local types = require('host.types')

local function colortype(a, ...)
	if a then
		return types.colortype(a, ...)
	end
end

rectangle.color_face = colortype(0.2, 0.2, 0.2, 1)
rectangle.color_border = colortype(1, 1, 1, 1)

function rectangle:init(it, parent, prefix)
	prefix = prefix or ''
	control.init(self, it, parent, prefix)
	local sfentry = self.gifield.multisf:getresourceentry('')
	self.borderwidth = it[prefix .. 'borderwidth'] or 0
	if self.borderwidth > 0 then
		self.bordersprite = sfentry.spritefield:newsprite()
	end
	self.innersprite = sfentry.spritefield:newsprite()
	self.color_face = colortype(it[prefix .. 'color_face'])
	self.color_border = colortype(it[prefix .. 'color_border'])
	rectangle.updatecolor(self)
	rectangle.updateposition(self)
end

function rectangle:updateposition()
	control.updateposition(self)
	local left, bottom, width, height = self.position:getglobalpos()
	local inner_left, inner_bottom, inner_right, inner_top =
		self.clipregion:clipvalues(
			left + self.borderwidth, bottom + self.borderwidth,
			width - 2*self.borderwidth, height - 2*self.borderwidth)
	local inner_x = left + self.borderwidth + (inner_left + inner_right)/2
	local inner_y = bottom + self.borderwidth + (inner_bottom + inner_top)/2
	local inner_width = inner_right - inner_left
	local inner_height = inner_top - inner_bottom
	self.innersprite:setposition(inner_x, inner_y)
	self.innersprite:setextent(inner_width/2, 0, 0, inner_height/2)
	if self.borderwidth > 0 then
		local outer_left, outer_bottom, outer_right, outer_top =
			self.clipregion:clipvalues(
				left, bottom,
				width, height)
		local outer_x = left + (outer_left + outer_right)/2
		local outer_y = bottom + (outer_bottom + outer_top)/2
		local outer_width = outer_right - outer_left
		local outer_height = outer_top - outer_bottom
		self.bordersprite:setposition(outer_x, outer_y)
		self.bordersprite:setextent(outer_width/2, 0, 0, outer_height/2)
	end
end

function rectangle:updatecolor()
	if self.bordersprite then
		self.bordersprite:setcolor(self.color_border)
	end
	self.innersprite:setcolor(self.color_face)
end

