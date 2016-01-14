local modname = ...
local baseoi = package.relrequire(modname, 2, 'base.instance')
local identityoi = baseoi:module(modname)
local fulltype

function identityoi:init(pr)
	baseoi.init(self, pr)
	self.context = pr.context
	self.args = pr.args
	self.fulltype = fulltype:create(pr.ti, false, true)
end

function identityoi:rcompile(stream)
	if not self.retname then
		self.retname = self.args[1]:rcompile(stream)
	end
	return self.retname
end

fulltype = package.relrequire(modname, 4, 'fulltype')
