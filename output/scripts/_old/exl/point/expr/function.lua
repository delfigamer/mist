local modname = ...
local base = require('exl.point.expr.base')
local exlcontext
local functionv
local namespace
local point
local functionex = base:module(modname, 'expr.function')

function functionex:init(pr, context)
	base.init(self, pr, context)
	self.namespace = namespace:create(context.environment)
	self.innercontext = exlcontext:create{
		parent = context,
		environment = self.namespace,
		visibility = 'private'}
	self.rettype = point:create(pr._rettype, self.innercontext)
	self.arglist = point:create(pr._arglist, self.innercontext)
	self.body = point:create(pr._body, self.innercontext)
	self.value = functionv:create{
		context = context,
		innercontext = self.innercontext,
		rettype = self.rettype:getvalue(),
		args = self.arglist:getargs(),
		body = self.body}
end

function functionex:defstring(lp)
	return 'function '..self.rettype:defstring(lp)..
		self.arglist:defstring(lp)..
		self.body:defstring(lp..self.tabchar)..'\
'..lp..'end'
end

exlcontext = require('exl.context')
functionv = require('exl.value.function')
namespace = require('exl.namespace')
point = require('exl.point.base')
