local modname = ...
local object = package.relrequire(modname, 2, 'object')
local node = object:module(modname)

function node:init(pr)
	self.spos = pr.spos
	self.epos = pr.epos
	self.filename = pr.filename
end

function node:build(pc)
end
