local modname = ...
local block = package.relrequire(modname, 2, 'block')
local classbody = block:module(modname)
local classbodycontext
local common

function classbody:init(pr)
	block.init(self, pr)
end

function classbody:dobuild(pc)
	self.context = classbodycontext:create(pc)
	for i, stat in ipairs(self.statements) do
		stat:build(self.context)
	end
end

classbodycontext = package.relrequire(modname, 0, 'context')
common = package.relrequire(modname, 3, 'common')
