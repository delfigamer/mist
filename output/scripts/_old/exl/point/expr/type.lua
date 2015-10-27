local modname = ...
local base = require('exl.point.expr.base')
local typeex = base:module(modname, 'expr.type')
local point

function typeex:init(pr, context)
	base.init(self, pr, context)
	self.target = point:create(pr._target, context)
	self.value = self.target.value
end

function typeex:defstring(lp)
	return 'type '..self.target:defstring(lp)
end

point = require('exl.point.base')
