local modname = ...
local invprototyped = require(
	modname, 3, 'operator.prototyped.invocation')
local invcustom = invprototyped:module(modname)
local common
local opfunctioncall

invcustom.castrank = 30

function invcustom:init(it)
	invprototyped.init(self, it)
	local ref = common.createnode{
		name = 'expr.reference',
		context = it.context,
		spos = it.spos,
		epos = it.spos,
		filename = it.filename,
		target = self.operator.symbol,
	}
	local dcargs = {ref}
	for i, arg in ipairs(it.args) do
		dcargs[i + 1] = arg
	end
	self.functioncall = opfunctioncall:invoke{
		context = it.context,
		spos = it.spos,
		epos = it.epos,
		filename = it.filename,
		context = it.context,
		opname = 'call',
		args = dcargs,
	}
end

function invcustom:rcompile(stream)
	if not self.retname then
		self.retname = self.functioncall:rcompile(stream)
	end
	return self.retname
end

common = require(modname, 4, 'common')
opfunctioncall = require(
	modname, 3, 'operator.functioncall.operator')
