local modname = ...
local baseoi = require(modname, 2, 'base.instance')
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
		stream:append{
			[0]='instantiate',
			{[0]='ssa', base}, -- class
			{[0]='ssa', self.retname}, -- target
		}
	end
	return self.retname
end

fulltype = require(modname, 4, 'fulltype')
