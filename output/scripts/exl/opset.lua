local modname = ...
local object = require('exl.object')
local opset = object:module(modname)
local common

function opset:init()
	self.functions = {}
end

function opset:insert(prototype, operator)
	for i, item in ipairs(self.functions) do
		if item.prototype:iseq(prototype) then
			return false, 'duplicate operator'..operator, operator.spos
		end
	end
	table.append(self.functions, {
		prototype = prototype,
		operator = operator,
	})
	return true
end

function opset:resolve(prototype)
	for i, item in ipairs(self.functions) do
		if item.prototype:istypeeq(prototype) then
			return item.operator
		end
	end
end

function opset:defstring(lp)
	-- local slines = {}
	-- for i, item in table.spairs(self.list) do
		-- slines[i] = '\n' .. lp .. common.defstring(item.operator, lp)
	-- end
	-- return table.concat(slines)
	return 'opset'
end

common = require('exl.common')
