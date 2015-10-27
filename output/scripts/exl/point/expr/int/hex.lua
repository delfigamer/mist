local modname = ...
local intbaseexpr = require('exl.point.expr.int.base')
local inthexexpr = intbaseexpr:module(modname)
local common

function inthexexpr:init(pr, context)
	intbaseexpr.init(self, pr, context)
	self.intvalue = tonumber(pr._whole, 16)
	if pr._sign == '-' then
		self.intvalue = self.intvalue * -1
	end
end

common = require('exl.common')
