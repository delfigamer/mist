local modname = ...
local coloredshape = require('host.coloredshape')
local lineshape = coloredshape:module(modname, 'lineshape', 'LineShape', 0xb7ed546e)
local ffi = require('ffi')
local host = require('host')
local ml = require('host.methodlist').graphics
local types = require('host.types')

function lineshape:create()
	local ptr = ml.lineshape_new()
	if ptr ~= nil then
		return self:new(ptr)
	else
		host.checkerror()
		return nil
	end
end

function lineshape:getpos()
	local pos = types.linepostype()
	if not ml.lineshape_getpos(self.ptr, pos) then
		host.checkerror()
		return nil
	end
	return unpack(pos, 0, 5)
end

function lineshape:setpos(r,...)
	local pos
	if type(r) == 'table' then
		pos = types.linepostype(r)
	else
		pos = types.linepostype(r,...)
	end
	if not ml.lineshape_setpos(self.ptr, pos) then
		host.checkerror()
	end
end
