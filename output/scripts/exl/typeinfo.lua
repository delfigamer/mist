local modname = ...
local object = require('exl.object')
local typeinfo = object:module(modname)
-- local opset

function typeinfo:init()
	-- self.opset = opset:create()
end

-- function typeinfo:getopset()
	-- return self.opset
-- end

function typeinfo:iseq(other)
	return self == other
end

-- opset = require('exl.opset')
