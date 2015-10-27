local modname = ...
local base = require('exl.point.expr.base')
local identref
local identifier = base:module(modname, 'expr.identifier')

function identifier:init(pr, context)
	base.init(self, pr, context)
	self.name = pr._name
	self.value = identref:create{
		context = context,
		name = self.name}
end

function identifier:defstring(lp)
	return self.name
end

identref = require('exl.value.identref')
