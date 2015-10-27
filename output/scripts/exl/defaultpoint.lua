local modname = ...
local point = require('exl.point')
local defaultpoint = point:module(modname)

function defaultpoint:init(pr, context)
	self.pr = pr
end

function defaultpoint:defstring(lp)
	return string.format('< %s (unknown) >', self.pr.name)
end
