local modname = ...
local ebase = require(modname, 3, 'expr.base')
local invbase = ebase:module(modname)

function invbase:init(it)
	ebase.init(self, it)
	self.operator = it.operator
end

function invbase:getoperator()
	return self.operator
end

function invbase:prefcompare(other)
	return 0
end
