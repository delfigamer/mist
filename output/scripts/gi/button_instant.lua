local modname = ...
local button = require('gi.button')
local button_instant = button:module(modname)

function button_instant:handle_pointdown(message, point, x, y)
end

function button_instant:handle_pointup(message, point)
end

function button_instant:handle_pointenter(message, point)
	if point ~= 0 then
		self.onclick(self, point)
		self.pdown[point] = true
	end
	self.phover[point] = true
	self:updatecolor()
end

function button_instant:handle_pointleave(message, point)
	if point ~= 0 then
		self.pdown[point] = nil
	end
	self.phover[point] = nil
	self:updatecolor()
end
