local modname = ...
local basearg = package.relrequire(modname, 1, 'base')
local intarg = basearg:module(modname)
local scalars = require('rs.scalars')

function intarg:init(it)
	basearg.init(self, it)
	if type(it[2]) ~= 'number' then
		error('number expected')
	end
	self.value = it[2]
end

function intarg:write(stream)
	scalars.int:write(self.value, stream)
end

function intarg:read(stream)
	local value = scalars.int:read(stream)
	return intarg:create{'ssa', value}
end

function intarg:defstring(lp)
	return string.format('%i', self.value)
end
