local modname = ...
local node = require('exl.node')
local ebinary = node:module(modname)
local common

local symbols = {
	add = '+',
	sub = '-',
}

function ebinary:init(pr)
	node.init(self, pr)
	self.operator = pr.operator
	self.left = pr.left
	self.right = pr.right
end

function ebinary:defstring(lp)
	return string.format('%s %s %s',
		self.left:defstring(lp .. self.lpindent),
		symbols[self.operator],
		self.right:defstring(lp .. self.lpindent))
end

common = require('exl.common')
