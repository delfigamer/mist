local modname = ...
local node = require(modname, 2, 'base')
local ebase = node:module(modname)
local common

function ebase:derive(...)
	local child = node.derive(self, ...)
	return child
end

function ebase:init(pr)
	node.init(self, pr)
end

function ebase:getfulltype()
	return self.fulltype or ebase.fulltype
end

function ebase:getconstvalue()
	return self.constvalue
end

function ebase:gettivalue()
	local cv = self:getconstvalue()
	if cv and cv ~= self then
		return cv:gettivalue()
	end
end

common = require(modname, 3, 'common')

local default = require(modname, 0, 'ti')
local fulltype = require(modname, 3, 'fulltype')

ebase.fulltype = fulltype:create(default, false, false)
