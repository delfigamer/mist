local modname = ...
local baseoi = package.relrequire(modname, 2, 'base.instance')
local defassignoi = baseoi:module(modname)
local fulltype

function defassignoi:init(pr)
	baseoi.init(self, pr)
	self.context = pr.context
	self.args = pr.args
	self.fulltype = fulltype:create(pr.ti, false, true)
end

function defassignoi:rcompile(stream)
	if not self.retname then
		self.retname = self.args[2]:rcompile(stream)
		self.args[1]:lcompile(stream, self.retname)
	end
	return self.retname
end

fulltype = package.relrequire(modname, 4, 'fulltype')
