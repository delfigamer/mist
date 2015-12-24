local modname = ...
local csymbase = package.relrequire(modname, 1, 'base')
local csymlocal = csymbase:module(modname)
local common

function csymlocal:init(it)
	csymbase.init(self, it)
	self.id = it.context:genid()
end

function csymlocal:lcompile(stream, source)
	stream:writetoken('a_setselfl', self.id, source)
end

function csymlocal:rcompile(stream)
	local name = stream:genname()
	stream:writetoken('a_getselfl', name, self.id)
	return name
end

function csymlocal:defstring(lp)
	if self.fulltype then
		return string.format('%s local %s',
			self.id, self.fulltype:defstring(lp))
	else
		return string.format('%s local <error>',
			self.id)
	end
end

common = package.relrequire(modname, 4, 'common')
