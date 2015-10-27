local modname = ...
local rectangle = require('gi.rectangle')
local button = rectangle:module(modname)
local labelbase = require('gi.labelbase')
local types = require('host.types')

local function colortype(a, ...)
	if a then
		return types.colortype(a, ...)
	end
end

button.color_idle = colortype(0.2, 0.2, 0.2, 1)
button.color_idle_border = colortype(1, 1, 1, 1)
button.color_hover = colortype(0.3, 0.3, 0.3, 1)
button.color_hover_border = colortype(1, 1, 1, 1)
button.color_down = colortype(0.25, 0.25, 0.25, 1)
button.color_down_border = colortype(1, 1, 1, 1)

function button:init(it, parent, prefix)
	prefix = prefix or ''
	rectangle.init(self, it, parent, prefix)
	self.color_face_idle = colortype(it[prefix .. 'color_face_idle'])
	self.color_face_hover = colortype(it[prefix .. 'color_face_hover'])
	self.color_face_down = colortype(it[prefix .. 'color_face_down'])
	self.color_border_idle = colortype(it[prefix .. 'color_border_idle'])
	self.color_border_hover = colortype(it[prefix .. 'color_border_hover'])
	self.color_border_down = colortype(it[prefix .. 'color_border_down'])
	self.pdown = {}
	self.phover = {}
	self:updatecolor()
	self.label = labelbase:create(it, self, 'label_')
	self:updateposition()
end

function button:release()
	self.label:release()
end

function button:setactive(active)
	self.label:setactive()
	if not active then
		self.pdown = {}
		self.phover = {}
	end
end

function button:updateposition()
	rectangle.updateposition(self)
	self.label:updateposition()
end

function button:updatecolor()
	if next(self.phover) then
		if next(self.pdown) then
			self.color_face = self.color_face_down
			self.color_border = self.color_border_down
		else
			self.color_face = self.color_face_hover
			self.color_border = self.color_border_hover
		end
	else
		self.color_face = self.color_face_idle
		self.color_border = self.color_border_idle
	end
	rectangle.updatecolor(self)
end

function button:adjustwidth()
	local lleft, lbottom, lwidth, lheight, lright, ltop = self.label.position:getlocalpos()
	self.position.width = lleft - lright + self.label.local_width
	self:updateposition()
end

function button:adjustheight()
	local lleft, lbottom, lwidth, lheight, lright, ltop = self.label.position:getlocalpos()
	self.position.height = lbottom - ltop + self.label.local_height
	self:updateposition()
end

function button:handle_pointdown(message, point, x, y)
	self.pdown[point] = true
	self:updatecolor()
end

function button:handle_pointup(message, point)
	if self.phover[point] then
		self.onclick(self, point)
	end
	self.pdown[point] = nil
	self:updatecolor()
end

function button:handle_pointenter(message, point)
	self.phover[point] = true
	self:updatecolor()
end

function button:handle_pointleave(message, point)
	self.phover[point] = nil
	self:updatecolor()
end

function button.onclick(sender, point)
end
