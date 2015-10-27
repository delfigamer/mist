local modname = ...
local point = require('exl.point.base')
local base = point:module(modname)

function base:init(pr, context)
	point.init(self, pr)
	self.visibility = context.visibility
end

function base:defstring(lp)
	return '<definition point>'
end

function base:compile_luai(lp)
	return '--[[definition point]]'
end
