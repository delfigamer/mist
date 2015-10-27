local modname = ...
local base = require('exl.type.base')
local unitscope
local unitt = base:module(modname)

function unitt:init(it)
	base.init(self, it)
	self.unit = it.unit
end

function unitt:scope(it)
	return unitscope:create{
		context = it.context,
		base = it.base,
		index = it.index,
		unit = self.unit}
end

function unitt:defstring(lp)
	return 'unit'
end

unitscope = require('exl.type.unit.scope')
