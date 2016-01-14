local modname = ...
local ebase = package.relrequire(modname, 1, 'base')
local eliteral = ebase:module(modname)
local common
local fulltype
local literalti

function eliteral:derive(target, modname, tipr)
	local class = ebase.derive(self, target, modname)
	local ti = literalti:create(tipr)
	class.fulltype = fulltype:create(ti, false, true)
	return class
end

function eliteral:init(pr)
	ebase.init(self, pr)
	self.constvalue = self
end

common = package.relrequire(modname, 3, 'common')
fulltype = package.relrequire(modname, 3, 'fulltype')
literalti = package.relrequire(modname, 0, 'ti')
