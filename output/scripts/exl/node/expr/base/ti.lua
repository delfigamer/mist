local modname = ...
local object = package.relrequire(modname, 4, 'object')
local typeinfo = object:module(modname)
local assignof
local common
local identityof
local initof

typeinfo.serial = '< error >'

function typeinfo:init()
end

function typeinfo:iseq(other)
	return self == other
end

function typeinfo:internalresolve(op, proto)
	if op == 'assign' then
		return assignof
	elseif op == 'init' then
		return initof
	elseif op == 'identity' then
		return identityof
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
identityof = package.relrequire(modname, 3, 'operator.identity.factory')
initof = package.relrequire(modname, 3, 'operator.init.factory')
