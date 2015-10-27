local modname = ...
local base = require('exl.point.expr.base')
local intv
local decint = base:module(modname, 'expr.decint')

function decint:init(pr, context)
	base.init(self, pr, context)
	self.sign = pr._sign
	self.digits = pr._digits
	self.intvalue = tonumber(self.digits)
	if self.sign == '-' then
		self.intvalue = -self.intvalue
	end
	self.value = intv:create{
		context = context,
		intvalue = self.intvalue}
end

function decint:defstring(lp)
	return self.sign..self.digits
end

intv = require('exl.value.int')
