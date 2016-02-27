local modname = ...
local opprototyped = require(modname, 3,
	'operator.prototyped.operator')
local opfunctorcall = opprototyped:module(modname)
local common

function opfunctorcall:init(it)
	opprototyped.init(self, it)
	self.targetname = it.targetname
	self.userargs = it.userargs
	self.symbol = it.symbol
end

function opfunctorcall:defstring(lp)
	local argstr = {}
	for i, arg in ipairs(self.userargs) do
		argstr[i] = arg:defstring(lp .. self.lpindent)
	end
	if self.retfulltype.rvalue then
		return string.format('function %s(%s): %s',
			common.identstring(self.targetname),
			table.concat(argstr, ', '),
			self.retfulltype.ti:defstring(lp .. self.lpindent))
	else
		return string.format('function %s(%s)',
			common.identstring(self.targetname),
			table.concat(argstr, ', '))
	end
end

common = require(modname, 4, 'common')
opfunctorcall.invocationclass = require(modname, 1, 'invcall')
