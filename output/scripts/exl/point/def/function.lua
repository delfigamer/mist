local modname = ...
local point = require('exl.point')
local functiondef = point:module(modname)
local common

function functiondef:init(pr, context)
	self.name = pr._name
	self.body = common.createpoint(pr._body, context)
end

function functiondef:defstring(lp)
	return string.format('function %s%s\n%send',
		self.name, self.body:defstring(lp .. self.lpindent), lp)
end

common = require('exl.common')
