local modname = ...
local node = require('exl.node')
local ebase = node:module(modname)
local common
local fulltype

function ebase:init(pr)
	node.init(self, pr)
end

function ebase:getfulltype()
	return self.fulltype
end

function ebase:getconstvalue()
	return self.constvalue
end

function ebase:islvalue()
	return false
end

function ebase:isrvalue()
	return true
end

function ebase:gettivalue()
	local cv = self:getconstvalue()
	if cv and cv ~= self then
		return cv:gettivalue()
	end
end

common = require('exl.common')
fulltype = require('exl.fulltype')

ebase.fulltype = fulltype:create(nil, false, true)
