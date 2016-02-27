local modname = ...
local object = require(modname, 4, 'object')
local baseof = object:module(modname)

function baseof:init(it)
	self.defpos = it.defpos
	self.deffile = it.deffile
end

function baseof:invoke(it)
end
