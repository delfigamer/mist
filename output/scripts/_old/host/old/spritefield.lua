local modname = ...
local shape = require('host.shape')
local spritefield = shape:module(modname, 'spritefield', 'SpriteField', 0xe166eadf)
local ffi = require('ffi')
local host = require('host')
local ml = require('host.methodlist').graphics
local sprite = require('host.sprite')
local types = require('host.types')

function spritefield:create()
	local ptr = ml.spritefield_new()
	if ptr ~= nil then
		return self:new(ptr)
	else
		host.checkerror()
		return nil
	end
end

function spritefield:setpixelprogram(pixelprogram)
	if not ml.spritefield_setpixelprogram(self.ptr, pixelprogram and pixelprogram.ptr) then
		host.checkerror()
	end
end

function spritefield:newsprite(order)
	local ptr = ml.spritefield_newsprite(self.ptr, order or 0)
	if ptr ~= nil then
		return sprite:new(ptr, self.ptr)
	else
		host.checkerror()
		return nil
	end
end

function spritefield:setblendmethod(method)
	if not ml.spritefield_setblendmethod(self.ptr, method) then
		host.checkerror()
	end
end

function spritefield:settexture(stage, texture)
	if not ml.spritefield_settexture(self.ptr, stage, texture and texture.ptr) then
		host.checkerror()
	end
end

function spritefield:setmatrix(matrix)
	if not ml.spritefield_setmatrix(self.ptr, matrix) then
		host.checkerror()
	end
end

spritefield.Method_Replace = 0
spritefield.Method_AlphaBlend = 1
spritefield.Method_Add = 2
spritefield.Method_AddSaturated = 3
spritefield.Method_Invalid = 4
