local modname = ...
local object = require('base.object')
local context = object:module(modname)

function context:init(it)
	if it.parent then
		object.copy(it.parent, self)
	end
	object.copy(it, self)
end
