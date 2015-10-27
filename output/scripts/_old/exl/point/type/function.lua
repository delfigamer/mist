local modname = ...
local base = require('exl.point.expr.base')
local exlcontext
local functiont
local namespace
local point
local functiontypeex = base:module(modname, 'type.function')

function functiontypeex:init(pr, context)
	base.init(self, pr, context)
	self.namespace = namespace:create(context.environment)
	self.innercontext = exlcontext:create{
		parent = context,
		environment = self.namespace,
		visibility = 'private'}
	self.rettype = point:create(pr._rettype, self.innercontext)
	self.arglist = point:create(pr._arglist, self.innercontext)
	self.value = functiont:create{
		context = context,
		rettype = self.rettype:getvalue(),
		args = self.arglist:getargs()}
end

function functiontypeex:defstring(lp)
	return 'function '..self.rettype:defstring(lp)..
		self.arglist:defstring(lp)
end

exlcontext = require('exl.context')
functiont = require('exl.type.function')
namespace = require('exl.namespace')
point = require('exl.point.base')
