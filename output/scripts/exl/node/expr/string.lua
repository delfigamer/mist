local modname = ...
local node = require('exl.node')
local estring = node:module(modname)
local common

function estring:init(pr)
	node.init(self, pr)
	self.value = pr.value
end

function estring:defstring(lp)
	return string.format('%q', self.value)
end

common = require('exl.common')
