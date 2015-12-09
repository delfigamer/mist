local modname = ...
local object = require('exl.object')
local aofunc = object:module(modname)
local aoinstance

function aofunc:init(pr)
end

function aofunc:createinstance(it)
	local lv = it.args[1]
	local rv = it.args[2]
	local lft = lv:getfulltype()
	local rft = rv:getfulltype()
	if not lft.lvalue
			or not rft.rvalue
			or not lft.ti:iseq(rft.ti) then
		return
	end
	local spos = it.args[1].spos
	local epos = it.args[2].epos
	return aoinstance:create{
		spos = spos,
		epos = epos,
		context = it.context,
		args = it.args,
		ti = lft.ti,
	}
end

aoinstance = require('exl.node.expr.defassign.instance')
