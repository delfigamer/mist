local modname = ...
local node = require('mistsh.node')
local defaultnode = node:module(modname)

function defaultnode:init(pr, context)
	node.init(self, pr, context)
	self.pr = pr
end

function defaultnode:defstring(lp)
	return string.format('< %s >', self.pr:defstring(lp))
end
