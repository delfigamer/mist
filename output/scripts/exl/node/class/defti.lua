local modname = ...
local baseti = package.relrequire(modname, 2, 'expr.base.ti')
local classdefti = baseti:module(modname)
local classdefcallof
local common

function classdefti:init(classinfo)
	baseti.init(self)
	self.classinfo = classinfo
	self.serial = 'c' .. common.identserial(self.classinfo:getclassname())
end

function classdefti:iseq(other)
	if not other['#' .. modname] then
		return false
	end
	return self.classinfo == other.classinfo
end

function classdefti:internalresolve(op, proto)
	if op == 'call' then
		return classdefcallof
	else
		return baseti.internalresolve(self, op, proto)
	end
end

function classdefti:defstring(lp)
	return string.format('type %s', self.classinfo:getclassname())
end

classdefcallof = package.relrequire(modname, 2, 'operator.classdefcall.factory')
common = package.relrequire(modname, 3, 'common')
