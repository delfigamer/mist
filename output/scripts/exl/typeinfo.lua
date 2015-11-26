local modname = ...
local object = require('base.object')
local typeinfo = object:module(modname)
local opset

function typeinfo:init()
	self.opset = opset:create()
end

function typeinfo:getopset()
	return self.opset
end

function typeinfo:defstring(lp)
	return '< ' .. self._NAME .. ' >'
end

function typeinfo.instmeta:__tostring()
	return self:defstring('')
end

opset = require('exl.opset')
