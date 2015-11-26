local modname = ...
local object = require('exl.object')
local node = object:module(modname)

function node:init(pr)
	self.spos = pr.spos
	self.epos = pr.epos
end

function node:build(pc)
end
