local modname = ...
local texture = require('host.texture')
local rendertarget = texture:module(modname, 'rendertarget', 'RenderTarget')
local ffi = require('ffi')
local host = require('host')
local ml = require('host.methodlist').graphics

if ml == nil then
	error('graphics are not supported')
end

function rendertarget:create(width, height)
	local ptr = ml.rendertarget_new(width, height)
	if ptr ~= nil then
		return self:new(ptr)
	else
		host.checkerror()
		return nil
	end
end

function rendertarget:setdepthstenciluse(use)
	if not ml.rendertarget_setdepthstenciluse(self.ptr, use) then
		host.checkerror()
	end
end

function rendertarget:setshape(shape)
	if not ml.rendertarget_setshape(self.ptr, shape and shape.ptr) then
		host.checkerror()
	end
end

function rendertarget:setclearcolor(flag, value)
	if not ml.rendertarget_setclearcolor(self.ptr, flag, value) then
		host.checkerror()
	end
end

function rendertarget:setcleardepth(flag, value)
	if not ml.rendertarget_setcleardepth(self.ptr, flag, value) then
		host.checkerror()
	end
end

function rendertarget:setclearstencil(flag, value)
	if not ml.rendertarget_setclearstencil(self.ptr, flag, value) then
		host.checkerror()
	end
end
