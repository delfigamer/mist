local modname = ...
local invbase = package.relrequire(modname, 2, 'base.invocation')
local invprototyped = invbase:module(modname)

function invprototyped:init(pr)
	invbase.init(self, pr)
	self.args = pr.args
	self.fulltype = pr.fulltype
end

function invprototyped:rcompile(stream)
	return 0
end
