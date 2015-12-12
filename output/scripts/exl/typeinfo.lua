local modname = ...
local object = require('exl.object')
local typeinfo = object:module(modname)
local aofunc

function typeinfo:init()
end

function typeinfo:iseq(other)
	return self == other
end

function typeinfo:getdefaultopfunc(op, proto)
	if op == 'assign' then
		return aofunc
	end
end

function typeinfo:getserial()
	return self.serial
end

aofunc = require('exl.node.expr.defassign.func')
