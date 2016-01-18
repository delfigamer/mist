local modname = ...
local object = package.relrequire(modname, 4, 'object')
local baseof = object:module(modname)

function baseof:init(it)
	self.defpos = it.defpos
	self.deffile = it.deffile
end

function baseof:invoke(it)
end
