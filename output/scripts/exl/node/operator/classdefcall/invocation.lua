local modname = ...
local invbase = require(modname, 2, 'base.invocation')
local invclasscall = invbase:module(modname)
local fulltype

function invclasscall:init(pr)
	invbase.init(self, pr)
	-- self.context = pr.context
	self.base = pr.base
	self.fulltype = fulltype:create(pr.ti, false, true)
end

function invclasscall:rcompile(stream)
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
