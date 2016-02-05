local modname = ...
local invprototyped = package.relrequire(
	modname, 3, 'operator.prototyped.invocation')
local invfunctorcall = invprototyped:module(modname)
local common
local opfunctioncall

function invfunctorcall:init(it)
	invprototyped.init(self, it)
	local args = {}
	args[1] = common.createnode{
		name = 'expr.reference',
		context = it.context,
		spos = it.spos,
		epos = it.spos,
		filename = it.filename,
		target = self.operator.symbol,
	}
	for i = 2, #it.args do
		args[i] = it.args[i]
	end
	self.functioncall = opfunctioncall:invoke{
		context = it.context,
		spos = it.spos,
		epos = it.epos,
		filename = it.filename,
		args = args,
	}
end

function invfunctorcall:rcompile(stream)
	if not self.retname then
		self.retname = self.functioncall:rcompile(stream)
	end
	return self.retname
end

common = package.relrequire(modname, 4, 'common')
opfunctioncall = package.relrequire(
	modname, 3, 'operator.functioncall.operator')
