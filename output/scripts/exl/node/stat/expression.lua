local modname = ...
local node = package.relrequire(modname, 2, 'base')
local sexpression = node:module(modname)
local common

function sexpression:init(pr)
	node.init(self, pr)
	self.value = pr.value
end

function sexpression:build(pc)
	self.value:build(pc)
end

function sexpression:compile(stream)
	self.value:rcompile(stream)
end

function sexpression:defstring(lp)
	return self.value:defstring(lp .. self.lpindent)
end

common = package.relrequire(modname, 3, 'common')
