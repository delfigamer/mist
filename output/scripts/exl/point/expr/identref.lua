local modname = ...
local point = require('exl.point')
local identrefexpr = point:module(modname)
local common

function identrefexpr:init(pr, context)
	self.target = pr._target
end

function identrefexpr:defstring(lp)
	return string.format('%s', self.target)
end

common = require('exl.common')
