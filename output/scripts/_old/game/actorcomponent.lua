local modname = ...
local object = require('base.object')
local actorcomponent = object:module(modname)

function actorcomponent:create()
	error('use actor:createcomponent()')
end
