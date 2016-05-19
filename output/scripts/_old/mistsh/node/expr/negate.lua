local modname = ...
local node = require('mistsh.node')
local negateexpr = node:module(modname)
local common

function negateexpr:init(pr, context)
	node.init(self, pr, context)
	self.value = common.createnode(pr._value, context)
end

function negateexpr:defstring(lp)
	return
		'( - ' .. self.value:defstring(lp .. self.lpindent) .. ' )'
end

function negateexpr:build(section)
	return
		'( - ' .. self.value:build(section) .. ' )'
end

common = require('mistsh.common')
