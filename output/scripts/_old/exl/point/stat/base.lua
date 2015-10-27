local modname = ...
local point = require('exl.point.base')
local base = point:module(modname)

function base:init(pr, context)
	point.init(self, pr)
end

function base:defstring(lp)
	return '<statement point>'
end

function base:compile_luai(lp)
	return '--[[statement point]]'
end
