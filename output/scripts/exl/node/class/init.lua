local modname = ...
local object = require(modname, 2, 'object')
local classinfo = object:module(modname)
local classcontext
local classdefti
local classinstanceti

function classinfo:init(it)
	object.init(self, it)
	self.outercontext = it.context
	self.classname = it.classname
	local nameprefix = it.context:getnameprefix()
	if nameprefix then
		self.classname = nameprefix .. self.classname
	end
	self.parent = it.parent
	self.context = classcontext:create(nil, it.context)
	self.defti = classdefti:create(self)
	self.instanceti = classinstanceti:create(self)
end

function classinfo:getclassname()
	return self.classname
end

function classinfo:getcontext()
	return self.context
end

function classinfo:getdefti()
	return self.defti
end

function classinfo:getinstanceti()
	return self.instanceti
end

classcontext = require(modname, 0, 'context')
classdefti = require(modname, 0, 'defti')
classinstanceti = require(modname, 0, 'instanceti')
