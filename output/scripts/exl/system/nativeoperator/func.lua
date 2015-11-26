local modname = ...
local object = require('exl.object')
local nofunc = object:module(modname)
local noinstance

function nofunc:init(pr)
	self.rettype = pr.rettype
	self.constfunc = pr.constfunc
	self.opcode = pr.opcode
end

function nofunc:createinstance(args)
	local spos, epos
	if args[1] then
		spos = args[1].spos
		epos = args[#args].epos
	end
	return noinstance:create{
		nofunc = self,
		args = args,
		context = args.context,
		spos = spos,
		epos = epos,
	}
end

noinstance = require('exl.system.nativeoperator.instance')
