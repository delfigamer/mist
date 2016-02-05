local modname = ...
local basearg = package.relrequire(modname, 1, 'base')
local numberarg = basearg:module(modname)
local scalars = require('rs.scalars')

numberarg.type = 'number'

function numberarg:init(it)
	basearg.init(self, it)
	if type(it[2]) ~= 'number' then
		error('number expected')
	end
	self.value = it[2]
end

function numberarg:write(stream)
	scalars.double:write(self.value, stream)
end

function numberarg:read(stream)
	self.value = scalars.double:read(stream)
end

function numberarg:defstring(lp)
	return string.format('%s', self.value)
end
