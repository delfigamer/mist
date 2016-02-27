local modname = ...
local ebase = require(modname, 1, 'base')
local eliteral = ebase:module(modname)
local common
local fulltype
local literalti

eliteral.bsimplevalue = true

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

common = require(modname, 3, 'common')
fulltype = require(modname, 3, 'fulltype')
literalti = require(modname, 0, 'ti')
