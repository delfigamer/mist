local modname = ...
local shape = require('host.shape')
local ffi = require('ffi')
local host = require('host')
local ml = require('host.methodlist').graphics
local types = require('host.types')
local coloredshape = shape:module(modname, 'coloredshape', 'ColoredShape', 0x2fe14ca5)

function coloredshape:getcolor()
	local color = types.colortype()
	if not ml.coloredshape_getcolor(self.ptr, color) then
		host.checkerror()
		return nil
	end
	return color[0], color[1], color[2], color[3]
end

function coloredshape:setcolor(r,g,b,a)
	local color
	if type(r) == 'table' then
		color = types.colortype(r)
	else
		color = types.colortype(r,g,b,a)
	end
	if not ml.coloredshape_setcolor(self.ptr, color) then
		host.checkerror()
	end
end

coloredshape:buildmetatype()
