local modname = ...
local point = require('exl.point')
local localdef = point:module(modname)
local common

function localdef:init(pr, context)
	self.name = pr._name
	self.type = common.createpoint(pr._type, context)
end

function localdef:defstring(lp)
	return string.format('local %s %s',
		self.type:defstring(lp .. self.lpindent), self.name)
end

common = require('exl.common')
