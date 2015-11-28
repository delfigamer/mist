local modname = ...
local ebase = require('exl.node.expr.base')
local etypebase = ebase:module(modname)
local common

function etypebase:init(pr)
	ebase.init(self, pr)
	self.typeinfo = pr.typeinfo
end

function etypebase:build(pc)
	if self.typedef then
		self.typedef:build(pc)
	end
end

function etypebase:getconstvalue()
	return self
end

function etypebase:islvalue()
	return false
end

function etypebase:isrvalue()
	return true
end

function etypebase:rcompile(stream)
	return 0
end

function etypebase:gettivalue()
	return self.typeinfo
end

function etypebase:defstring(lp)
	return common.defstring(self.typeinfo, lp)
end

common = require('exl.common')
