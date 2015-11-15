local modname = ...
local object = require('base.object')
local astnode = object:module(modname)

function astnode:create(it, ...)
	setmetatable(it, self.instmeta)
	it:init(...)
	return it
end
