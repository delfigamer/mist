local modname = ...
local ebase = require(modname, 3, 'expr.base')
local invbase = ebase:module(modname)

invbase.castrank = 0
invbase.maxrank = 0

function invbase:init(it)
	ebase.init(self, it)
	self.operator = it.operator
end

function invbase:getoperator()
	return self.operator
end

function invbase:getcastrank()
	return self.castrank
end

function invbase:getmaxrank()
	return self.maxrank
end
