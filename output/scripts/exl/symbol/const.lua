local modname = ...
local symlocal = require('exl.symbol.local')
local symconst = symlocal:module(modname)
local common

function symconst:init(it)
	symlocal.init(self, it)
	self.constvalue = it.constvalue
end

function symconst:getconstvalue()
	return self.constvalue
end

function symconst:defstring(lp)
	if self.constvalue then
		return string.format('%s const %s',
			self.id, common.defstring(self.constvalue, lp))
	elseif self.type then
		return string.format('%s const forward %s',
			self.id, common.defstring(self.fulltype, lp))
	else
		return string.format('%s const <error>',
			self.id)
	end
end

common = require('exl.common')
