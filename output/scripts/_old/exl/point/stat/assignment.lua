local modname = ...
local statbase = require('exl.point.stat.base')
local point
local assignment = statbase:module(modname, 'stat.assignment')

function assignment:init(pr, context)
	statbase.init(self, pr, context)
	self.lvalue = point:create(pr._lvalue, context)
	self.rvalue = point:create(pr._rvalue, context)
	self.vlvalue = self.lvalue:getvalue()
	assert(self.vlvalue.type, 'lvalue must have a type')
	self.vrvalue = self.vlvalue.type:acast(self.rvalue:getvalue())
end

function assignment:defstring(lp)
	return self.lvalue:defstring(lp)..' = '..
		self.rvalue:defstring(lp)
end

point = require('exl.point.base')
