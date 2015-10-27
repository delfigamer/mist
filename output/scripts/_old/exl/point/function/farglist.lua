local modname = ...
local point = require('exl.point.base')
local farglist = point:module(modname, 'function.farglist')

function farglist:init(pr, context)
	point.init(self, pr)
	self.argpoints = {}
	for i, ppr in ipairs(pr._args) do
		self.argpoints[i] = point:create(ppr, context, i)
	end
end

function farglist:getargs()
	local result = {}
	for i, point in ipairs(self.argpoints) do
		result[i] = point.entity
	end
	return result
end

function farglist:defstring(lp)
	local lines = {}
	for i, point in ipairs(self.argpoints) do
		lines[i] = tostring(point)
	end
	return '('..table.concat(lines, ', ')..')'
end
