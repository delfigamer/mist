local modname = ...
local esymbolbase = require('exl.node.expr.symbolbase')
local enewident = esymbolbase:module(modname)
local common
local fulltype

function enewident:init(pr)
	esymbolbase.init(self, pr)
	self.target = pr.target
	if self.target then
		local ft = self.target:getfulltype()
		self.fulltype = fulltype:create(ft.ti, true, false)
	else
		self.fulltype = fulltype:create(nil, true, false)
	end
end

function enewident:build(pc)
end

function enewident:defstring(lp)
	return common.defstring(self.target, lp)
end

common = require('exl.common')
fulltype = require('exl.fulltype')
