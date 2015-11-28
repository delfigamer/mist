local modname = ...
local object = require('exl.object')
local symbase = object:module(modname)
local fulltype

function symbase:init(it)
	self.context = it.context
	self.defpos = it.defpos
	self.fulltype = it.fulltype
	self.constvalue = it.constvalue
end

function symbase:getfulltype()
	return self.fulltype
end

function symbase:getconstvalue()
	return self.constvalue
end

fulltype = require('exl.fulltype')

symbase.fulltype = fulltype:create(nil, false, true)
