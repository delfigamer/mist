local modname = ...
local node = require('exl.node')
local defaultnode = node:module(modname)

function defaultnode:init(pr)
	self.pr = pr
end

function defaultnode:defstring(lp)
	return string.format('< %s (unknown) >', self.pr.name)
end
