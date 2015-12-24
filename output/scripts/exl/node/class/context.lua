local modname = ...
local context = package.relrequire(modname, 3, 'context')
local classcontext = context:module(modname)

function classcontext:init(parent, outer)
	context.init(self, parent)
	if not self.env and outer then
		self.env = outer.env
	end
	self.outer = outer
end
