local modname = ...
local node = require(modname, 1, 'base')
local defaultnode = node:module(modname)

function defaultnode:init(pr)
	node.init(self, pr)
	self.pr = pr
end

function defaultnode:defstring(lp)
	return string.format('< %s (unknown) >', self.pr.name)
end
