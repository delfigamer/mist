local modname = ...
local ebase = require('exl.node.expr.base')
local etypebase = ebase:module(modname)
local common
local fulltype

function etypebase:init(pr)
	ebase.init(self, pr)
	self.typeinfo = pr.typeinfo
	self.constvalue = self
end

-- function etypebase:build(pc)
	-- if self.typedef then
		-- self.typedef:build(pc)
	-- end
-- end

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
fulltype = require('exl.fulltype')

etypebase.fulltype = fulltype:create(nil, false, true)
