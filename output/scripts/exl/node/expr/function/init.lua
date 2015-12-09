local modname = ...
local efunctionbase = require('exl.node.expr.function.base')
local efunction = efunctionbase:module(modname)
local common
local context
local exlbstream
local fulltype
local functionti

function efunction:build(pc)
	efunctionbase.build(self, pc)
	self.body:build(self.context)
end

function efunction:defstring(lp)
	if self.rettype then
		return string.format('function%s: %s%s\n%send',
			common.defstring(self.arglist, lp .. self.lpindent),
			common.defstring(self.rettype, lp .. self.lpindent),
			common.defstring(self.body, lp .. self.lpindent),
			lp)
	else
		return string.format('function%s%s\n%send',
			common.defstring(self.arglist, lp .. self.lpindent),
			common.defstring(self.body, lp .. self.lpindent),
			lp)
	end
end

common = require('exl.common')
context = require('exl.context')
exlbstream = require('exlb.stream')
fulltype = require('exl.fulltype')
functionti = require('exl.node.expr.function.ti')
