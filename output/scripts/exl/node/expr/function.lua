local modname = ...
local node = require('exl.node')
local efunction = node:module(modname)
local common

function efunction:init(pr)
	node.init(self, pr)
	self.rettype = pr.rettype
	self.body = pr.body
end

function efunction:defstring(lp)
	return string.format('function %s()%s\n%send',
		common.defstring(self.rettype, lp .. self.lpindent),
		common.defstring(self.body, lp .. self.lpindent),
		lp)
end

common = require('exl.common')
