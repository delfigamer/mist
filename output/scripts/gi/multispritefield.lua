local modname = ...
local object = require('base.object')
local multisf = object:module(modname)
local ffi = require('ffi')
local info = require('host.info')
local pixelprogram = require('host.pixelprogram')
local resourceset = require('rs.resourceset')
local spritefield = require('host.spritefield')
local texture = require('host.texture')

function multisf:init(shapegroup, shapeorder)
	self.shapegroup = shapegroup
	self.shapeorder = shapeorder
	self.mainpp = pixelprogram:create()
	self.mainpp:setstagecolor(0, pixelprogram.Op_Modulate, pixelprogram.Arg_Texture, pixelprogram.Arg_Diffuse)
	self.entries = {}
	local entry = {
		pixelprogram = pixelprogram:create(),
		spritefield = spritefield:create(),
	}
	entry.sfiterator = self.shapegroup:insert(entry.spritefield, self.shapeorder)
	entry.pixelprogram:setstagecolor(0, pixelprogram.Op_SelectArg1, pixelprogram.Arg_Diffuse, pixelprogram.Arg_Current)
	entry.spritefield:setpixelprogram(entry.pixelprogram)
	entry.spritefield:setmatrix(ffi.new('float[16]',
		2/info.width, 0, 0, 0,
		0, 2/info.height, 0, 0,
		0, 0, 1, 0,
		-info.texelsoffset*2/info.width-1, -info.texeltoffset*2/info.height-1, 0, 1))
	self.entries[''] = entry
end

function multisf:createentry(texobject)
	local entry = {
		texture = texobject,
		spritefield = spritefield:create(),
	}
	entry.texture:setminfilter(texture.MinFilter_Nearest)
	entry.texture:setmagfilter(texture.MagFilter_Nearest)
	entry.sfiterator = self.shapegroup:insert(entry.spritefield, self.shapeorder)
	entry.spritefield:setpixelprogram(self.mainpp)
	entry.spritefield:setblendmethod(entry.spritefield.Method_AlphaBlend)
	entry.spritefield:settexture(0, entry.texture)
	entry.spritefield:setmatrix(ffi.new('float[16]',
		2/info.width, 0, 0, 0,
		0, 2/info.height, 0, 0,
		0, 0, 1, 0,
		-info.texelsoffset*2/info.width-1, -info.texeltoffset*2/info.height-1, 0, 1))
	return entry
end

function multisf:getresourceentry(texname)
	local entry =  self.entries[texname]
	if not entry then
		entry = self:createentry(resourceset:loadresource(texname, 'Texture'):gettexture())
		self.entries[texname] = entry
	end
	return entry
end

function multisf:getruntimeentry(texobject)
	local entry =  self.entries[texobject]
	if not entry then
		entry = self:createentry(texobject)
		self.entries[texobject] = entry
	end
	return entry
end
