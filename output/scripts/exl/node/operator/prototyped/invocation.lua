local modname = ...
local invbase = require(modname, 2, 'base.invocation')
local invprototyped = invbase:module(modname)

function invprototyped:init(it)
	invbase.init(self, it)
	self.args = it.args
	self.fulltype = it.fulltype
	self.maxrank = it.maxrank
end

function invprototyped:rcompile(stream)
	return 0
end
