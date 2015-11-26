local modname = ...
local object = require('base.object')
local proto = object:module(modname)
local common

function proto:init(args)
	for i, arg in ipairs(args) do
		self[i] = {
			ti = arg.ti,
			lvalue = arg.lvalue,
			rvalue = arg.rvalue,
		}
	end
end

function proto.iseq(a, b)
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

function proto.istypeeq(a, b)
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

common = require('exl.common')
