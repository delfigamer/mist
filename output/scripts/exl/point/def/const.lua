local modname = ...
local point = require('exl.point')
local constdef = point:module(modname)
local common

function constdef:init(pr, context)
	self.name = pr._name
	self.value = common.createpoint(pr._value, context)
end

function constdef:defstring(lp)
	return string.format('const %s = %s',
		self.name, self.value:defstring(lp .. self.lpindent))
end

common = require('exl.common')
