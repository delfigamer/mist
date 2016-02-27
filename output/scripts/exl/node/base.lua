local modname = ...
local object = require(modname, 2, 'object')
local node = object:module(modname)

function node:init(pr)
	self.spos = pr.spos
	self.epos = pr.epos
	self.filename = pr.filename
end

function node:build(pc, ...)
	if not self.balreadybuilt then
		self.balreadybuilt = true
		self:dobuild(pc, ...)
	end
end

function node:dobuild(pc)
end
