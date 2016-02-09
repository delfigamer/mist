local modname = ...
local opprototyped = package.relrequire(modname, 3,
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

common = package.relrequire(modname, 4, 'common')
opfunctorcast.invocationclass = package.relrequire(modname, 1, 'invcast')
