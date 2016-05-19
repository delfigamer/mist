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
	self.entries = {}
end

function multisf:createentry(texobject)
	local entry = {
		texture = texobject,
		spritefield = spritefield:create(),
	}
	if self.minfilter then
		entry.texture:setminfilter(self.minfilter)
	end
	if self.magfilter then
		entry.texture:setmagfilter(self.magfilter)
	end
	entry.sfiterator = self.shapegroup:insert(entry.spritefield, self.shapeorder)
	entry.spritefield:setpixelprogram(self.mainpp)
	if self.blendmethod then
		entry.spritefield:setblendmethod(self.blendmethod)
	end
	entry.spritefield:settexture(0, entry.texture)
	if self.fieldmatrix then
		entry.spritefield:setmatrix(self.fieldmatrix)
	end
	return entry
end

function multisf:getresourceentry(texname)
	local entry = self.entries[texname]
	if not entry then
		entry = self:createentry(resourceset:loadresource(texname, 'Texture'):gettexture())
		self.entries[texname] = entry
	end
	return entry
end

-- function multisf:getruntimeentry(texobject)
	-- local entry = self.entries[texobject]
	-- if not entry then
		-- entry = self:createentry(texobject)
		-- self.entries[texobject] = entry
	-- end
	-- return entry
-- end
