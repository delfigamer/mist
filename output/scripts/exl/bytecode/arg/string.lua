local modname = ...
local basearg = package.relrequire(modname, 1, 'base')
local stringarg = basearg:module(modname)
local scalars = require('rs.scalars')

function stringarg:init(it)
	basearg.init(self, it)
	self.value = it[2]
end

function stringarg:write(stream)
	scalars.string:write(self.value, stream)
end

function stringarg:read(stream)
	local value = scalars.string:read(stream)
	return stringarg:create{'string', value}
end

function stringarg:defstring(lp)
	return string.format('%q', self.value)
end
