local modname = ...
local object = require('base.object')
local mixer = object:module(modname)
local set = require('base.set')

function mixer:init(it)
	self.nodes = set:create()
	for i, node in ipairs(it) do
		self.nodes:insert(node)
	end
end

function mixer:reset()
	for i, node in self.nodes:pairs() do
		node:reset()
	end
end

function mixer:next()
	local value = 0
	for i, node in self.nodes:pairs() do
		value = value + node:next()
	end
	return value
end

function mixer:insert(node)
	self.nodes:insert(node)
end

function mixer:remove(node)
	self.nodes:remove(node)
end
