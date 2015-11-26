local modname = ...
local object = require('base.object')
local opset = object:module(modname)
local common

function opset:init(parent)
	self.parent = parent
	if parent then
		self.env = parent.env
	end
	self.operators = {}
end

function opset:insert(opname, prototype, operator)
	local list = table.provide(self.operators, opname)
	for i, item in ipairs(list) do
		if item.prototype:iseq(prototype) then
			self.env:log('error', 'duplicate operator'..operator, operator.spos)
		end
	end
	table.append(list, {
		prototype = prototype,
		operator = operator,
	})
end

function opset:resolve(opname, prototype)
	local list = self.operators[opname]
	if list then
		for i, item in ipairs(list) do
			if item.prototype:istypeeq(prototype) then
				return item.operator
			end
		end
	end
	if self.parent then
		return self.parent:resolve(opname, prototype)
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

function opset.instmeta:__tostring()
	return self:defstring('')
end

common = require('exl.common')
