local modname = ...
local invbase = require(modname, 2, 'base.invocation')
local invassign = invbase:module(modname)
local fulltype

function invassign:init(it)
	invbase.init(self, it)
	self.args = it.args
	self.fulltype = fulltype:create(it.ti, false, true)
	self.rank = it.rank
end

function invassign:rcompile(stream)
	if not self.retname then
		self.retname = self.args[2]:rcompile(stream)
		self.args[1]:lcompile(stream, self.retname)
	end
	return self.retname
end

function invbase:prefcompare(other)
	return self.args[2]:prefcompare(other.arg[2])
end

fulltype = require(modname, 4, 'fulltype')
