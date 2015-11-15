local modname = ...
local node = require('exl.node')
local esubexpression = node:module(modname)
local common

function esubexpression:init(pr)
	node.init(self, pr)
	self.value = pr.value
end

function esubexpression:defstring(lp)
	return string.format('(%s)',
		common.defstring(self.value, lp .. self.lpindent))
end

common = require('exl.common')
