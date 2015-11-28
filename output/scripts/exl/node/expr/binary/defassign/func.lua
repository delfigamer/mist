local modname = ...
local object = require('exl.object')
local aofunc = object:module(modname)
local aoinstance

function aofunc:init(pr)
end

function aofunc:createinstance(args)
	local lv = args[1]
	local rv = args[2]
	local lft = lv:getfulltype()
	local rft = rv:getfulltype()
	if not lft.lvalue
			or not rft.rvalue
			or not lft.ti:iseq(rft.ti) then
		return
	end
	local spos = args[1].spos
	local epos = args[2].epos
	return aoinstance:create{
		spos = spos,
		epos = epos,
		context = args.context,
		args = args,
		ti = lft.ti,
	}
end

aoinstance = require('exl.node.expr.binary.defassign.instance')
