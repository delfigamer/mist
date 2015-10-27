local modname = ...
local point = require('exl.point')
local intbaseexpr = point:module(modname)
local common

function intbaseexpr:defstring(lp)
	return string.format('%i', self.intvalue)
end

common = require('exl.common')
