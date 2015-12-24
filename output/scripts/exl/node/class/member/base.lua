local modname = ...
local node = package.relrequire(modname, 3, 'base')
local cmbase = node:module(modname)
local common

function cmbase:init(pr)
	node.init(self, pr)
end

function cmbase:build(pc)
	self.bexternal = true
	self.bpublic = true
end

common = package.relrequire(modname, 4, 'common')
