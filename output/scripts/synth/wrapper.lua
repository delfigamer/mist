local modname = ...
local object = require('base.object')
local wrapper = object:module(modname)

function wrapper:init(gen, ...)
	self.gen = gen
	self.genargs = table.pack(...)
	self:reset()
end

function wrapper:reset()
	self.func = self.gen(table.unpack(self.genargs))
end

function wrapper:next()
	return self.func()
end
