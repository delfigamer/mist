local modname = ...
local esymbolbase = require(modname, 1, 'symbolbase')
local ereference = esymbolbase:module(modname)
local common

function ereference:init(pr)
	esymbolbase.init(self, pr)
	self.target = pr.target
	esymbolbase.dobuild(self, pr.context)
end

common = require(modname, 3, 'common')
