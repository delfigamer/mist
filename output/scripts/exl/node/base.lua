local modname = ...
local object = package.relrequire(modname, 2, 'object')
local node = object:module(modname)

function node:init(pr)
	self.spos = pr.spos
	self.epos = pr.epos
	self.filename = pr.filename
end

function node:build(...)
	if not self.balreadybuilt then
		self.balreadybuilt = true
		self:dobuild(...)
	end
end

function node:dobuild(pc)
end
