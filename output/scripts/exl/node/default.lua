local modname = ...
local node = package.relrequire(modname, 1, 'base')
local defaultnode = node:module(modname)

function defaultnode:init(pr)
	node.init(self, pr)
	self.pr = pr
end

function defaultnode:defstring(lp)
	return string.format('< %s (unknown) >', self.pr.name)
end
