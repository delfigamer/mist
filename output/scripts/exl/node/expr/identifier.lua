local modname = ...
local node = require('exl.node')
local eidentifier = node:module(modname)
local common

function eidentifier:init(pr)
	node.init(self, pr)
	self.targetname = pr.targetname
end

function eidentifier:defstring(lp)
	return self.targetname
end

common = require('exl.common')
