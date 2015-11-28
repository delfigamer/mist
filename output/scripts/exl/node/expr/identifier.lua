local modname = ...
local esymbolbase = require('exl.node.expr.symbolbase')
local eidentifier = esymbolbase:module(modname)
local common

function eidentifier:init(pr)
	esymbolbase.init(self, pr)
	self.targetname = pr.targetname
end

function eidentifier:build(pc)
	self.target = pc:getsymbol(self.targetname)
	if not self.target then
		pc.env:log(
			'error',
			string.format('unknown identifier: %s', self.targetname),
			self.spos, self.epos)
		return
	end
	esymbolbase.build(self, pc)
end

function eidentifier:defstring(lp)
	return self.targetname
end

common = require('exl.common')
