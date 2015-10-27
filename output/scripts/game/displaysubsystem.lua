local modname = ...
local object = require('base.object')
local displaysubsystem = object:module(modname)
local ffi
local info
local pixelprogram
local shapegroup
local spritefield

function displaysubsystem:init()
	self.shapegroup = shapegroup:create()
	self.pixelprogram = pixelprogram:create()
	self.spritefield = spritefield:create()
	self.sfiterator = self.shapegroup:insert(self.spritefield)
	self.pixelprogram:setstagecolor(0, pixelprogram.Op_SelectArg1, pixelprogram.Arg_Diffuse, pixelprogram.Arg_Current)
	self.spritefield:setpixelprogram(self.pixelprogram)
	self.spritefield:setmatrix(ffi.new('float[16]',
		2/info.width, 0, 0, 0,
		0, 2/info.height, 0, 0,
		0, 0, 1, 0,
		---info.texelsoffset*2/info.width-1, -info.texeltoffset*2/info.height-1, 0, 1
		0, 0, 0, 1
	))
end

ffi = require('ffi')
info = require('host.info')
pixelprogram = require('host.pixelprogram')
shapegroup = require('host.shapegroup')
spritefield = require('host.spritefield')
