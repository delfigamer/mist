local modname = ...
local baseoi = package.relrequire(modname, 2, 'base.instance')
local classcalloi = baseoi:module(modname)
local fulltype

function classcalloi:init(pr)
	baseoi.init(self, pr)
	self.context = pr.context
	self.base = pr.base
	self.fulltype = fulltype:create(pr.ti, false, true)
end

function classcalloi:rcompile(stream)
	if self.retname == nil then
		local base = self.base:rcompile(stream)
		self.retname = stream:genname()
		stream:writetoken('a_instance', self.retname, base)
	end
	return self.retname
end

fulltype = package.relrequire(modname, 4, 'fulltype')
