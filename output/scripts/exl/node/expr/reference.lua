local modname = ...
local esymbolbase = package.relrequire(modname, 1, 'symbolbase')
local ereference = esymbolbase:module(modname)
local common

function ereference:init(pr)
	esymbolbase.init(self, pr)
	self.target = pr.target
	esymbolbase.dobuild(self, pr.context)
end

common = package.relrequire(modname, 3, 'common')
