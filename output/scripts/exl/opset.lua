local modname = ...
local object = require('exl.object')
local opset = object:module(modname)
local common

local function prototypeeq(a, b)
	if #a ~= #b then
		return false
	end
	for i, aarg in ipairs(a) do
		local barg = b[i]
		if aarg.ti ~= barg.ti then
			return false
		elseif aarg.lvalue and not barg.lvalue then
			return false
		elseif aarg.rvalue and not barg.rvalue then
			return false
		end
	end
	return true
end

function protoexacteq(a, b)
	if #a ~= #b then
		return false
	end
	for i, aarg in ipairs(a) do
		local barg = b[i]
		if aarg.ti ~= barg.ti then
			return false
		elseif aarg.lvalue ~= barg.lvalue then
			return false
		elseif aarg.rvalue ~= barg.rvalue then
			return false
		end
	end
	return true
end

function opset:init()
	self.functions = {}
end

function opset:insert(prototype, operator)
	for i, item in ipairs(self.functions) do
		if protoexacteq(item.prototype, prototype) then
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
	local candidates = {}
	for i, item in ipairs(self.functions) do
		if prototypeeq(item.prototype, prototype) then
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
