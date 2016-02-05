local modname = ...
local opprototyped = package.relrequire(
	modname, 3, 'operator.prototyped.operator')
local opcustom = opprototyped:module(modname)
local common

function opcustom:init(it)
	opprototyped.init(self, it)
	self.opname = it.opname
	self.userargs = it.userargs
	self.symbol = it.symbol
end

function opcustom:defstring(lp)
	local argstr = {}
	for i, arg in ipairs(self.userargs) do
		argstr[i] = arg:defstring(lp .. self.lpindent)
	end
	if self.retfulltype.rvalue then
		return string.format('operator %s(%s): %s',
			common.identstring(self.opname),
			table.concat(argstr, ', '),
			self.retfulltype.ti:defstring(lp .. self.lpindent))
	else
		return string.format('operator %s(%s)',
			common.identstring(self.opname),
			table.concat(argstr, ', '))
	end
end

common = package.relrequire(modname, 4, 'common')
opcustom.invocationclass = package.relrequire(
	modname, 1, 'invcustom')
