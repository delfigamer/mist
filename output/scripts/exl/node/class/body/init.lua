local modname = ...
local block = require(modname, 2, 'block')
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

classbodycontext = require(modname, 0, 'context')
common = require(modname, 3, 'common')
