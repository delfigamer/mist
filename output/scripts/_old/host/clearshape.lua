local modname = ...
local shape = require('host.shape')
local clearshape = shape:module(modname, 'clearshape', 'ClearShape')
local ffi = require('ffi')
local host = require('host')
local types = require('host.types')
local ml = require('host.methodlist').graphics

if ml == nil then
	error('graphics are not supported')
end

function clearshape:create()
	local ptr = ml.clearshape_new()
	if ptr ~= nil then
		return self:new(ptr)
	else
		host.checkerror()
		return nil
	end
end

function clearshape:getcolor()
	local color = types.color()
	if not ml.clearshape_getcolor(self.ptr, color) then
		host.checkerror()
		return nil
	end
	return color[0], color[1], color[2], color[3]
end

function clearshape:setcolor(r,g,b,a)
	local color
	if type(r) == 'table' then
		color = types.color(r)
	else
		color = types.color(r,g,b,a)
	end
	if not ml.clearshape_setcolor(self.ptr, color) then
		host.checkerror()
	end
end
