local modname = ...
local symbase = require(modname, 4, 'symbol.base')
local csymbase = symbase:module(modname)

function csymbase:init(it)
	symbase.init(self, it)
	self.bexternal = it.bexternal
	self.bpublic = it.bpublic
end
