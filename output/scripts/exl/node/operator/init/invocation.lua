local modname = ...
local invbase = package.relrequire(modname, 2, 'base.invocation')
local invinit = invbase:module(modname)
local common
local fulltype

function invinit:init(it)
	invbase.init(self, it)
	self.args = it.args
	if #it.args == 2 then
		self.node = common.createnode{
			name = 'expr.invoke',
			opname = 'assign',
			spos = it.spos,
			epos = it.epos,
			filename = it.filename,
			args = it.args,
		}
		self.node:build(it.context)
		self.fulltype = self.node:getfulltype()
		self.constvalue = self.node:getconstvalue()
		self.rank = self.node.invocation:getrank()
	end
end

function invinit:rcompile(stream)
	if not self.retname then
		if self.node then
			self.retname = self.node:rcompile(stream)
		else
			self.retname = 0
			self.args[1]:lcompile(stream, 0)
		end
	end
	return self.retname
end

common = package.relrequire(modname, 4, 'common')
fulltype = package.relrequire(modname, 4, 'fulltype')
