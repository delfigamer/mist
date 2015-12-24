local modname = ...
local symbase = package.relrequire(modname, 1, 'base')
local symlocal = symbase:module(modname)
local common

function symlocal:init(it)
	symbase.init(self, it)
	self.id = it.context:genid()
end

function symlocal:lcompile(stream, source)
	stream:writetoken('a_setl', self.id, source)
end

function symlocal:rcompile(stream)
	local name = stream:genname()
	stream:writetoken('a_getl', name, self.id)
	return name
end

function symlocal:defstring(lp)
	if self.fulltype then
		return string.format('%s local %s',
			self.id, common.defstring(self.fulltype, lp))
	else
		return string.format('%s local <error>',
			self.id)
	end
end

common = package.relrequire(modname, 2, 'common')
