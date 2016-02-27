local modname = ...
local csymbase = require(modname, 1, 'base')
local csymlocal = csymbase:module(modname)
local common

function csymlocal:init(it)
	csymbase.init(self, it)
	self.id = it.context:genid()
end

function csymlocal:lcompile(stream, source, basename)
	stream:writetoken{
		op = 'move',
		args = {
			{'ssa', source}, -- source
			{'member', -- target
				{'ssa', basename},
				self.id
			},
		},
	}
end

function csymlocal:rcompile(stream, basename)
	local name = stream:genname()
	stream:writetoken{
		op = 'move',
		args = {
			{'member', -- source
				{'ssa', basename},
				self.id
			},
			{'ssa', name}, -- target
		},
	}
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

common = require(modname, 4, 'common')
