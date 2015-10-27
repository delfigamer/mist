local modname = ...
local point = require('exl.point')
local sumexpr = point:module(modname)
local common

function sumexpr:init(pr, context)
	self.lhs = common.createpoint(pr.prefix, context)
	self.rhs = common.createpoint(pr.suffix, context)
	self.operation = pr.infix
end

function sumexpr:defstring(lp)
	return string.format('( %s %s %s )',
		self.lhs:defstring(lp .. self.lpindent),
		self.operation,
		self.rhs:defstring(lp .. self.lpindent))
end

common = require('exl.common')
