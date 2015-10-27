local modname = ...
local point = require('exl.point')
local body = point:module(modname)
local common

function body:init(pr, context)
	self.definitions = {}
	for i, def in ipairs(pr._definitions) do
		self.definitions[i] = common.createpoint(def, context)
	end
	self.statements = {}
	for i, stat in ipairs(pr._statements) do
		self.statements[i] = common.createpoint(stat, context)
	end
end

function body:defstring(lp)
	local deflines = {}
	for i, def in ipairs(self.definitions) do
		deflines[i] = ('\n' .. lp) .. def:defstring(lp)
	end
	local statlines = {}
	for i, stat in ipairs(self.statements) do
		statlines[i] = ('\n' .. lp) .. stat:defstring(lp)
	end
	return table.concat(deflines) .. table.concat(statlines)
end

common = require('exl.common')
