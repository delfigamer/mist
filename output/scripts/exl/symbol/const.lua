local modname = ...
local symlocal = require(modname, 1, 'local')
local symconst = symlocal:module(modname)
local common

function symconst:init(it)
	symlocal.init(self, it)
end

function symconst:defstring(lp)
	if self.constvalue then
		return string.format('%s const %s',
			self.id, self.constvalue:defstring(lp))
	else
		return string.format('%s const <error>',
			self.id)
	end
end

common = require(modname, 2, 'common')
