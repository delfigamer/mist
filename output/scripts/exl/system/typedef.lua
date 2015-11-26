local modname = ...
local ebase = require('exl.node.expr.base')
local etypedef = ebase:module(modname)
local common

function etypedef:init(pr)
	ebase.init(self, pr)
	self.typeinfo = pr.typeinfo
end

function etypedef:getconstvalue()
	return self
end

function etypedef:islvalue()
	return false
end

function etypedef:isrvalue()
	return false
end

function etypedef:gettivalue()
	return self.typeinfo
end

function etypedef:defstring(lp)
	return common.defstring(self.typeinfo, lp)
end

common = require('exl.common')
