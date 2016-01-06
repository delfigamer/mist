local modname = ...
local object = package.relrequire(modname, 4, 'object')
local typeinfo = object:module(modname)
local assignof
local common

typeinfo.serial = '< error >'

function typeinfo:init()
end

function typeinfo:iseq(other)
	return self == other
end

function typeinfo:internalresolve(op, proto)
	if op == 'assign' then
		return assignof
	end
end

function typeinfo:getserial()
	return self.serial
end

function typeinfo:defstring(lp)
	return self:getserial()
end

assignof = package.relrequire(modname, 3, 'operator.assign.factory')
common = package.relrequire(modname, 4, 'common')
