local modname = ...
local node = require('exl.node')
local sexpression = node:module(modname)
local common

function sexpression:init(pr)
	node.init(self, pr)
	self.value = pr.value
end

function sexpression:defstring(lp)
	return common.defstring(self.value, lp .. self.lpindent)
end

common = require('exl.common')
