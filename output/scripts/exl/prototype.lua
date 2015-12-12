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

common = require('exl.common')
