local modname = ...
local baseti = require(modname, 3, 'expr.base.ti')
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

common = require(modname, 4, 'common')
context = require(modname, 4, 'context')
