local modname = ...
local invprototyped = package.relrequire(
	modname, 3, 'operator.prototyped.invocation')
local invfunctorcast = invprototyped:module(modname)
local common

invfunctorcast.castrank = 10

function invfunctorcast:init(it)
	invprototyped.init(self, it)
	self.args = {
		it.args[1],
		common.createnode{
			name = 'expr.reference',
			context = it.context,
			spos = it.spos,
			epos = it.spos,
			filename = it.filename,
			target = self.operator.symbol,
		},
	}
end

function invfunctorcast:rcompile(stream)
	if not self.retname then
		local name = self.args[2]:rcompile(stream)
		self.args[1]:lcompile(stream, name)
		self.retname = 0
	end
	return 0
end

common = package.relrequire(modname, 4, 'common')
