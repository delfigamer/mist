local modname = ...
local invbase = require(modname, 2, 'base.invocation')
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
		self.maxrank = self.node.invocation:getmaxrank()
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

common = require(modname, 4, 'common')
fulltype = require(modname, 4, 'fulltype')
