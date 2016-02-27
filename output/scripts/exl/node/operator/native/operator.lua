local modname = ...
local opprototyped = require(modname, 2, 'prototyped.operator')
local opnative = opprototyped:module(modname)

function opnative:init(pr)
	opprototyped.init(self, pr)
	self.constfunc = pr.constfunc
	self.opcode = pr.opcode
end

opnative.invocationclass = require(modname, 1, 'invocation')
