local modname = ...
local object = require('base.object')
local symbase = object:module(modname)
local genid

symbase.lpindent = '\t'

function symbase:init(it)
	self.defpos = it.defpos
	-- self.name = it.name
	self.id = genid()
	self.type = it.type
	self.constvalue = it.constvalue
end

function symbase:gettype()
	return self.type
end

function symbase:getconstvalue()
	return self.constvalue
end

function symbase:defstring(lp)
	return '<symbol>'
end

function symbase.instmeta:__tostring()
	return self:defstring('')
end

genid = require('exl.genid')
