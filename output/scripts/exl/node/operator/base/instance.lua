local modname = ...
local ebase = package.relrequire(modname, 3, 'expr.base')
local baseoi = ebase:module(modname)

function baseoi:init(pr)
	ebase.init(self, pr)
end
