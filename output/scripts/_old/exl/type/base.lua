local modname = ...
local valuebase = require('exl.value.base')
local errors
local typebase = valuebase:module(modname)

typebase.isconst = true

function typebase:init(it)
	valuebase.init(self, it)
	self.error_scope_invalid = errors.scope_invalid{type = self}
end

function typebase:scope(base, index)
	error(self.error_scope_invalid)
end

function typebase:cast(source)
	local stype = assert(source.type, 'the value must have a type')
	if self:isequal(stype) then
		return source
	else
		return nil
	end
end

function typebase:acast(source)
	return self:cast(source) or
		error(errors.cast_failure{
			value = source,
			stype = source.type,
			ttype = self})
end

function typebase:isequal(other)
	return self == other
end

function typebase:defstring(lp)
	return '<type>'
end

errors = require('exl.errors')
