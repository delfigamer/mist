local modname = ...
local node = require(modname, 2, 'base')
local sexpression = node:module(modname)
local common

function sexpression:init(pr)
	node.init(self, pr)
	self.value = pr.value
end

function sexpression:dobuild(pc)
	self.value:build(pc)
end

function sexpression:compile(stream)
	self.value:rcompile(stream)
end

function sexpression:defstring(lp)
	return self.value:defstring(lp .. self.lpindent)
end

common = require(modname, 3, 'common')
