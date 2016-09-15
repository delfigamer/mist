local modname = ...
local object = require('base.object')
local filter = object:module(modname)
local set = require('base.set')

function filter:init(it)
	self.source = assert(it.source)
	self.func = assert(it.func)
end

function filter:reset()
	self.source:reset()
end

function filter:next()
	return self.func(self.source:next())
end
