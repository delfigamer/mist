local modname = ...
local intbaseexpr = require('exl.point.expr.int.base')
local intdecexpr = intbaseexpr:module(modname)
local common

function intdecexpr:init(pr, context)
	intbaseexpr.init(self, pr, context)
	self.intvalue = tonumber(pr._whole)
	if pr._sign == '-' then
		self.intvalue = self.intvalue * -1
	end
end

common = require('exl.common')
