local modname = ...
local symbase = require(modname, 1, 'base')
local symlocal = symbase:module(modname)
local common

function symlocal:init(it)
	symbase.init(self, it)
	self.id = it.context:genid()
end

function symlocal:lcompile(stream, source, basename)
	stream:append{
		[0]='move',
		{[0]='ssa', source}, -- source
		{[0]='local', self.id}, -- target
	}
end

function symlocal:rcompile(stream, basename)
	local name = stream:genname()
	stream:append{
		[0]='move',
		{[0]='local', self.id}, -- source
		{[0]='ssa', name}, -- target
	}
	return name
end

function symlocal:defstring(lp)
	if self.fulltype then
		return string.format('%s local %s',
			self.id, self.fulltype:defstring(lp))
	else
		return string.format('%s local <error>',
			self.id)
	end
end

common = require(modname, 2, 'common')
