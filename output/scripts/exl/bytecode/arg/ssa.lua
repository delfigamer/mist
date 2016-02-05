local modname = ...
local intarg = package.relrequire(modname, 1, 'int')
local ssaarg = intarg:module(modname)
local scalars = require('rs.scalars')

ssaarg.type = 'ssa'

function ssaarg:init(it)
	intarg.init(self, it)
end

function ssaarg:defstring(lp)
	if self.value == 0 then
		return '.nil'
	else
		return string.format('.%i', self.value)
	end
end
