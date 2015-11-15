local modname = ...
local node = require('exl.node')
local sassign = node:module(modname)
local common

function sassign:init(pr)
	node.init(self, pr)
	self.left = pr.left
	self.right = pr.right
end

function sassign:defstring(lp)
	return string.format('%s = %s',
		common.defstring(self.left, lp .. self.lpindent),
		common.defstring(self.right, lp .. self.lpindent))
end

common = require('exl.common')
