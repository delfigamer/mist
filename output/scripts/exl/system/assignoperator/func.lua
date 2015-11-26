local modname = ...
local object = require('exl.object')
local aofunc = object:module(modname)
local aoinstance

function aofunc:init(pr)
	self.rettype = pr.rettype
end

function aofunc:createinstance(args)
	local spos = args[1].spos
	local epos = args[2].epos
	return aoinstance:create{
		aofunc = self,
		args = args,
		context = args.context,
		spos = spos,
		epos = epos,
	}
end

aoinstance = require('exl.system.assignoperator.instance')
