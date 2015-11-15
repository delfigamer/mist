local modname = ...
local node = require('exl.node')
local block = node:module(modname)
local common
local namespace

function block:init(pr)
	node.init(self, pr)
	self.statements = pr.statements
end

function block:build(env)
	self.namespace = namespace:create()
	for i, stat in ipairs(self.statements) do
		stat:build(env, self.namespace)
	end
end

function block:defstring(lp)
	local statlines = {}
	for i, stat in ipairs(self.statements) do
		statlines[i] = ('\n' .. lp) .. stat:defstring(lp)
	end
	return table.concat(statlines)
end

common = require('exl.common')
namespace = require('exl.namespace')
