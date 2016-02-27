local modname = ...
local node = require(modname, 3, 'base')
local cmbase = node:module(modname)
local common

function cmbase:init(pr)
	node.init(self, pr)
end

function cmbase:dobuild(pc)
	self.bexternal = true
	self.bpublic = true
end

common = require(modname, 4, 'common')
