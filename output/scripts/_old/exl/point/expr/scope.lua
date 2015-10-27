local modname = ...
local base = require('exl.point.expr.base')
local scopev
local scopeex = base:module(modname)

function scopeex:init(pr, context, it)
	base.init(self, pr, context)
	self.base = it.base
	self.index = pr._name
	self.value = scopev:create{
		context = context,
		base = self.base:getvalue(),
		index = self.index}
end

function scopeex:defstring(lp)
	return self.base:defstring(lp)..'.'..self.index
end

scopev = require('exl.value.scope')
