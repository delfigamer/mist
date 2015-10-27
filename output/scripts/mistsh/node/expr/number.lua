local modname = ...
local node = require('mistsh.node')
local numberexpr = node:module(modname)
local common

function numberexpr:init(pr, context)
	node.init(self, pr, context)
	self.value = pr._value
end

function numberexpr:defstring(lp)
	return string.format('%s', self.value)
end

function numberexpr:build()
	return string.format('%s', self.value)
end

common = require('mistsh.common')
