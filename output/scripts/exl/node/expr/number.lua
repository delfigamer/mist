local modname = ...
local node = require('exl.node')
local enumber = node:module(modname)
local common

function enumber:init(pr)
	node.init(self, pr)
	self.value = pr.value
end

function enumber:defstring(lp)
	return tostring(self.value)
end

common = require('exl.common')
