local modname = ...
local invbase = package.relrequire(modname, 2, 'base.invocation')
local invidentity = invbase:module(modname)
local fulltype

function invidentity:init(it)
	invbase.init(self, it)
	-- self.context = it.context
	self.args = it.args
	self.fulltype = fulltype:create(it.ti, false, true)
	self.constvalue = self.args[1]:getconstvalue()
end

function invidentity:rcompile(stream)
	if not self.retname then
		self.retname = self.args[1]:rcompile(stream)
	end
	return self.retname
end

fulltype = package.relrequire(modname, 4, 'fulltype')
