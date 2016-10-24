local modname = ...
local invbase = require(modname, 2, 'base.invocation')
local invinit = invbase:module(modname)
local common
local fulltype

function invinit:init(it)
	invbase.init(self, it)
	self.args = it.args
	if #it.args == 2 then
		self.assignment = common.createnode{
			name = 'expr.invoke',
			opname = 'assign',
			spos = it.spos,
			epos = it.epos,
			filename = it.filename,
			args = it.args,
		}
		self.assignment:build(it.context)
		self.fulltype = self.assignment:getfulltype()
		self.constvalue = self.assignment:getconstvalue()
	end
end

function invinit:rcompile(stream)
	if not self.retname then
		if self.assignment then
			self.retname = self.assignment:rcompile(stream)
		else
			self.retname = 0
			self.args[1]:lcompile(stream, 0)
		end
	end
	return self.retname
end

function invinit:prefcompare(other)
	if self.assignment and other.assignment then
		return self.assignment:prefcompare(other.assignment)
	end
end

common = require(modname, 4, 'common')
fulltype = require(modname, 4, 'fulltype')
