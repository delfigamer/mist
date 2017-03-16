local modname = ...
local baseti = require(modname, 2, 'expr.base.ti')
local classdefti = baseti:module(modname)
local common
local context
local opclasscall

function classdefti:init(classinfo)
	baseti.init(self)
	self.classinfo = classinfo
	self.serial = 'c' .. common.identserial(self.classinfo:getclassname())
	self.context = context:create()
	table.append(self.context:getoperatorlist('call'), opclasscall)
end

function classdefti:iseq(other)
	if not other['#' .. modname] then
		return false
	end
	return self.classinfo == other.classinfo
end

function classdefti:defstring(lp)
	return string.format('type %s', self.classinfo:getclassname())
end

common = require(modname, 3, 'common')
context = require(modname, 3, 'context')
opclasscall = require(modname, 2, 'operator.classdefcall.operator')
