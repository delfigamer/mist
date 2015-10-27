local modname = ...
local node = require('mistsh.node')
local binaryexpr = node:module(modname)
local common

function binaryexpr:init(pr, context)
	node.init(self, pr, context)
	self.lhs = common.createnode(pr.prefix, context)
	self.rhs = common.createnode(pr.suffix, context)
end

function binaryexpr:defstring(lp)
	return
		'( ' .. self.lhs:defstring(lp .. self.lpindent) ..
		' ' .. self.optoken ..
		' ' .. self.rhs:defstring(lp .. self.lpindent) .. ' )'
end

function binaryexpr:build(section)
	return
		'( ' .. self.lhs:build(section) ..
		' ' .. self.optoken ..
		' ' .. self.rhs:build(section) .. ' )'
end

common = require('mistsh.common')
