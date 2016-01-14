local modname = ...
local baseti = package.relrequire(modname, 3, 'expr.base.ti')
local functorti = baseti:module(modname)
local common

function functorti:init(it)
	baseti.init(self)
	self.name = it.name
	self.serial = common.identserial(it.name)
end

function functorti:defstring(lp)
	return string.format('type %s', self.name)
end

common = package.relrequire(modname, 4, 'common')
