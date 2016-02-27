local modname = ...
local ebase = require(modname, 1, 'base')
local etypedef = ebase:module(modname)
local common

function etypedef:init(pr)
	ebase.init(self, pr)
	self.typeinfo = pr.typeinfo
	self.constvalue = self
end

function etypedef:rcompile(stream)
	return 0
end

function etypedef:gettivalue()
	return self.typeinfo
end

-- function etypedef:defstring(lp)
	-- return self.typeinfo:defstring(lp)
-- end

common = require(modname, 3, 'common')

local typedefti = require(modname, 0, 'ti')
local fulltype = require(modname, 3, 'fulltype')

etypedef.fulltype = fulltype:create(typedefti, false, true)
