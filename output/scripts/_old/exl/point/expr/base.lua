local modname = ...
local point = require('exl.point.base')
local base = point:module(modname)

function base:init(pr, context)
	point.init(self, pr, context)
end

function base:getvalue()
	return assert(self.value)
end

function base:defstring(lp)
	return '<expression point>'
end

function base:compile_luai(lp)
	return self.value:compile_luai(lp)
end
