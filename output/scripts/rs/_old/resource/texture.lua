local modname = ...
local base = require('rs.resource.base')
local texture = base:module(modname, 'Texture')
local exttable = require('rs.resource.exttable')
local pngformat = require('rs.format.png')
local statictexture = require('host.statictexture')

exttable['png'] = texture

texture.searchpath = '?.png'

function texture:init(...)
	base.init(self, ...)
	self.texture = statictexture:create()
end

function texture:createnew()
	error('cannot create texture resources')
end

function texture:gettexture()
-- 	self:finishloading()
	return self.texture
end
