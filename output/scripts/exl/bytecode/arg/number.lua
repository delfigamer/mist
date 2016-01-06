local modname = ...
local basearg = package.relrequire(modname, 1, 'base')
local numberarg = basearg:module(modname)
local scalars = require('rs.scalars')

function numberarg:init(it)
	basearg.init(self, it)
	self.value = it[2]
end

function numberarg:write(stream)
	scalars.double:write(self.value, stream)
end

function numberarg:read(stream)
	local value = scalars.double:read(stream)
	return numberarg:create{'number', value}
end

function numberarg:defstring(lp)
	return string.format('%s', self.value)
end
