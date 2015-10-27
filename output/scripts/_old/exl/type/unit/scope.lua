local modname = ...
local reference = require('exl.value.reference')
local unitscope = reference:module(modname)
local errors

function unitscope:init(it)
	reference.init(self, it)
	self.base = it.base
	self.index = it.index
	local target = it.unit.namespace:get(self.index) or
		error(errors.scope_undefined{
			value = self.base,
			type = self.base.type,
			name = self.index})
	self:settarget(target)
end

function unitscope:defstring(lp)
	return self.base:defstring(lp)..'.'..self.index
end

function unitscope:compile_luai(lp)
	return '('..self.base:compile_luai(lp)..').'..self.index
end

errors = require('exl.errors')
