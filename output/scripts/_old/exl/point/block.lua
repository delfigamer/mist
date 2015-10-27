local modname = ...
local point = require('exl.point.base')
local block = point:module(modname, 'block')

function block:init(pr, context)
	point.init(self, pr)
	self.points = {}
	for i, ppr in ipairs(pr._body) do
		self.points[i] = point:create(ppr, context)
	end
end

function block:defstring(lp)
	local lines = {}
	for i, point in ipairs(self.points) do
		lines[i] = '\n' .. lp .. point:defstring(lp)
	end
	return table.concat(lines)
end

function block:compile_luai(lp)
	local lines = {}
	for i, point in ipairs(self.points) do
		lines[i] = '\n' .. lp .. point:compile_luai(lp)
	end
	return table.concat(lines)
end
