local modname = ...
local opprototyped = require(modname, 3,
	'operator.prototyped.operator')
local opfunctorcast = opprototyped:module(modname)
local common

function opfunctorcast:init(it)
	opprototyped.init(self, it)
	self.targetname = it.targetname
	self.symbol = it.symbol
end

function opfunctorcast:defstring(lp)
	return string.format('operator cast(out %s, %s)',
		self.args[1].ti:defstring(lp .. self.lpindent),
		self.args[2].ti:defstring(lp .. self.lpindent))
end

common = require(modname, 4, 'common')
opfunctorcast.invocationclass = require(modname, 1, 'invcast')
