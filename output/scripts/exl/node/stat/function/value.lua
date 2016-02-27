local modname = ...
local ebase = require(modname, 3, 'expr.base')
local efunctor = ebase:module(modname)
local common

function efunctor:init(pr)
	ebase.init(self, pr)
	self.fulltype = pr.fulltype
	self.constvalue = self
end

function efunctor:rcompile(stream)
	return 0
end

common = require(modname, 4, 'common')
