local modname = ...
local base = require('exl.point.expr.base')
local nilv
local nilex = base:module(modname, 'expr.nil')

function nilex:init(pr, context)
	base.init(self, pr, context)
	self.value = nilv:create{context = context}
end

function nilex:defstring(lp)
	return 'nil'
end

nilv = require('exl.value.nil')
