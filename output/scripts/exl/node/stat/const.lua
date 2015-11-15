local modname = ...
local node = require('exl.node')
local sconst = node:module(modname)
local common

function sconst:init(pr)
	node.init(self, pr)
	self.targetname = pr.targetname
	self.value = pr.value
end

function sconst:defstring(lp)
	return string.format('const %s = %s',
		common.defstring(self.targetname, lp .. self.lpindent),
		common.defstring(self.value, lp .. self.lpindent))
end

common = require('exl.common')
