local modname = ...
local entitybase = require('exl.entity.base')
local vare = entitybase:module(modname)

function vare:init(it)
	entitybase.init(self, it)
	self.type = it.type:constvalue()
end

function vare:defstring(lp)
	return 'var '..self.type:defstring(lp)..' '..self.name
end
