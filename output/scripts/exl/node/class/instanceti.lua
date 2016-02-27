local modname = ...
local baseti = require(modname, 2, 'expr.base.ti')
local classinstanceti = baseti:module(modname)
local common
local einstancemember

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

function classinstanceti:scoperesolve(it)
	local context = self.classinfo:getcontext()
	local target = context:getsymbol(it.index)
	if not target then
		common.nodeerror(string.format('type %s has no member named %s',
			self, common.identstring(it.index)), it)
	end
	return einstancemember:create{
		context = it.context,
		spos = it.spos,
		epos = it.epos,
		filename = it.filename,
		base = it.base,
		index = it.index,
		target = target,
	}
end

function classinstanceti:defstring(lp)
	return self.classinfo:getclassname()
end

common = require(modname, 3, 'common')
einstancemember = require(modname, 1, 'expr.instancemember')
