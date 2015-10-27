local modname = ...
local object = require('base.object')
local errors
local namespace = object:module(modname)

function namespace:init(parent)
	self.parent = parent
	self.symbols = {}
end

function namespace:get(name)
	local sym = self.symbols[name]
	if sym then
		return sym
	elseif self.parent then
		return self.parent:get(name)
	else
		return nil
	end
end

function namespace:geta(name)
	return self:get(name) or
		error(errors.symbol_undefined{
			name = name})
end

function namespace:set(name, value)
	if self.symbols[name] then
		error(errors.symbol_redefined{
			name = name})
	else
		self.symbols[name] = value
	end
end

errors = require('exl.errors')
