local modname = ...
local node = require('exl.node')
local sconst = node:module(modname)
local common
local symconst

function sconst:init(pr)
	node.init(self, pr)
	self.targetname = pr.targetname
	self.value = pr.value
end

function sconst:build(env, namespace)
	self.symbol = symconst:create{
		defpos = self.epos,
		constvalue = self.value
	}
	if self.targetname then
		namespace:set(self.targetname, self.symbol)
	end
end

function sconst:defstring(lp)
	return string.format('const %s = %s',
		common.defstring(self.targetname, lp .. self.lpindent),
		common.defstring(self.value, lp .. self.lpindent))
end

common = require('exl.common')
symconst = require('exl.symbol.const')
