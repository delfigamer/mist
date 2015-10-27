local modname = ...
local texture = require('host.texture')
local statictexture = texture:module(modname, 'statictexture', 'StaticTexture')
local ffi = require('ffi')
local host = require('host')
local ml = require('host.methodlist').graphics

if ml == nil then
	error('graphics are not supported')
end

function statictexture:create()
	local ptr = ml.statictexture_new()
	if ptr ~= nil then
		return self:new(ptr)
	else
		host.checkerror()
		return nil
	end
end

function statictexture:assign(format, data, width, height)
	if not ml.statictexture_assign(self.ptr, format, data and data.ptr, width, height) then
		host.checkerror()
	end
end
