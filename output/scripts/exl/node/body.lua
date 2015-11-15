local modname = ...
local node = require('exl.node')
local body = node:module(modname)
local common

function body:init(pr)
	node.init(self, pr)
	self.statements = pr.statements
end

function body:defstring(lp)
	local statlines = {}
	for i, stat in ipairs(self.statements) do
		statlines[i] = ('\n' .. lp) .. stat:defstring(lp)
	end
	return table.concat(statlines)
end

common = require('exl.common')
