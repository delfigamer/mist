local modname = ...
local stringarg = package.relrequire(modname, 1, 'string')
local localarg = stringarg:module(modname)
local common
local scalars = require('rs.scalars')

localarg.type = 'local'

function localarg:init(it)
	stringarg.init(self, it)
end

function localarg:defstring(lp)
	return common.identstring(self.value)
end

common = package.relrequire(modname, 3, 'common')
