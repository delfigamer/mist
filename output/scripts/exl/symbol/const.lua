local modname = ...
local symlocal = package.relrequire(modname, 1, 'local')
local symconst = symlocal:module(modname)
local common

function symconst:init(it)
	symlocal.init(self, it)
end

function symconst:defstring(lp)
	if self.constvalue then
		return string.format('%s const %s',
			self.id, common.defstring(self.constvalue, lp))
	else
		return string.format('%s const <error>',
			self.id)
	end
end

common = package.relrequire(modname, 2, 'common')
