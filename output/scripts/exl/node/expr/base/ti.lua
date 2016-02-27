local modname = ...
local object = require(modname, 4, 'object')
local typeinfo = object:module(modname)
local common

typeinfo.serial = '< error >'

function typeinfo:init()
end

function typeinfo:iseq(other)
	return self == other
end

function typeinfo:getserial()
	return self.serial
end

function typeinfo:getcontext()
	return self.context
end

function typeinfo:defstring(lp)
	return self:getserial()
end

common = require(modname, 4, 'common')
