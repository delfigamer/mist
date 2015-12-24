local modname = ...
local ebase = package.relrequire(modname, 1, 'base')
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

function etypedef:defstring(lp)
	return self.typeinfo:defstring(lp)
end

common = package.relrequire(modname, 3, 'common')

local typedefti = package.relrequire(modname, 0, 'ti')
local fulltype = package.relrequire(modname, 3, 'fulltype')

etypedef.fulltype = fulltype:create(typedefti, false, true)
