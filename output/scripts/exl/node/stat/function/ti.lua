local modname = ...
local baseti = package.relrequire(modname, 3, 'expr.base.ti')
local functorti = baseti:module(modname)
local common
local context

function functorti:init(it)
	baseti.init(self)
	self.name = it.name
	self.serial = common.identserial(it.name)
	self.context = context:create(it.context)
end

function functorti:defstring(lp)
	return string.format('type %s', self.name)
end

common = package.relrequire(modname, 4, 'common')
context = package.relrequire(modname, 4, 'context')
