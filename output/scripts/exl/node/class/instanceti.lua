local modname = ...
local baseti = package.relrequire(modname, 2, 'expr.base.ti')
local classinstanceti = baseti:module(modname)
local common

function classinstanceti:init(classinfo)
	baseti.init(self)
	self.classinfo = classinfo
	self.serial = common.identserial(self.classinfo:getclassname())
end

function classinstanceti:iseq(other)
	if not other['#' .. modname] then
		return false
	end
	return self.classinfo == other.classinfo
end

common = package.relrequire(modname, 3, 'common')
