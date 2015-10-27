local modname = ...
local base = require('exl.value.base')
local namespace
local stdlibv
local unitt
local unitv = base:module(modname)

unitv.isconst = true

function unitv:init(it)
	base.init(self, it)
	self.point = it.point
	self.name = it.name
	self.parent = it.parent or stdlibv
	self.namespace = namespace:create(self.parent.namespace)
	self.type = unitt:create{
		context = it.context,
		unit = self}
	self.innername = 'user_'..self.name
end

function unitv:defstring(lp)
	return 'unit '..self.name
end

function unitv:compile_luai(lp)
	return self.innername
end

namespace = require('exl.namespace')
stdlibv = require('exl.stdlib')
unitt = require('exl.type.unit')
