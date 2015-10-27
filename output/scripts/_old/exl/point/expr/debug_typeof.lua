local modname = ...
local base = require('exl.point.expr.base')
local debug_typeof = base:module(modname, 'expr.debug_typeof')
local point

function debug_typeof:init(pr, context)
	base.init(self, pr, context)
	self.target = point:create(pr._target, context)
	self.value = self.target.value
	self.type = self.value.type
end

function debug_typeof:defstring(lp)
	return '__debug_typeof --[['..
		(self.type and self.type:defstring(lp) or '!no type!')..
		']] '..self.target:defstring(lp)
end

point = require('exl.point.base')
