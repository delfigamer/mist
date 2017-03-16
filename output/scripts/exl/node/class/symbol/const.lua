local modname = ...
local csymlocal = require(modname, 1, 'local')
local csymconst = csymlocal:module(modname)
local common

function csymconst:init(it)
	csymlocal.init(self, it)
end

function csymconst:defstring(lp)
	if self.constvalue then
		return string.format('%s const %s',
			self.id, self.constvalue:defstring(lp))
	else
		return string.format('%s const <error>',
			self.id)
	end
end

common = require(modname, 4, 'common')
