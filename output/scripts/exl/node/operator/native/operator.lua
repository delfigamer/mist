local modname = ...
local opprototyped = package.relrequire(modname, 2, 'prototyped.operator')
local opnative = opprototyped:module(modname)
-- local invnative

function opnative:init(pr)
	opprototyped.init(self, pr)
	self.constfunc = pr.constfunc
	self.opcode = pr.opcode
end

opnative.invocationclass = package.relrequire(modname, 1, 'invocation')
