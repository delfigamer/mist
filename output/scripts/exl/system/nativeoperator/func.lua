local modname = ...
local object = require('exl.object')
local nofunc = object:module(modname)
local noinstance

function nofunc:init(pr)
	self.rettype = pr.rettype
	self.constfunc = pr.constfunc
	self.opcode = pr.opcode
end

function nofunc:createinstance(it)
	local spos, epos
	if it.args[1] then
		spos = it.args[1].spos
		epos = it.args[#it.args].epos
	end
	return noinstance:create{
		nofunc = self,
		args = it.args,
		context = it.context,
		spos = spos,
		epos = epos,
	}
end

noinstance = require('exl.system.nativeoperator.instance')
