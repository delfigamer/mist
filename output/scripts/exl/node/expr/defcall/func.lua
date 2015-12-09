local modname = ...
local object = require('exl.object')
local dfunc = object:module(modname)
local dinstance

function dfunc:init(pr)
end

function dfunc:createinstance(it)
	local args = it.args
	local base = args[1]
	local bft = base:getfulltype()
	if not bft.ti or not bft.ti['#exl.node.expr.function.ti'] then
		return
	elseif not bft.ti.arglist then
		return
	elseif #bft.ti.arglist.args ~= #args - 1 then
		return
	end
	local outargs = {}
	for i = 1, #args - 1 do
		local aarg = args[i+1]
		local farg = bft.ti.arglist.args[i]
		if not aarg or not farg or not farg.typev then
			return
		end
		local aargft = aarg:getfulltype()
		local fargti = farg.typev:gettivalue()
		if not aargft or not fargti then
			return
		elseif not aargft.lvalue and farg.blvalue then
			return
		elseif not aargft.rvalue and farg.brvalue then
			return
		elseif not aargft.ti or not fargti or not aargft.ti:iseq(fargti) then
			return
		end
		if farg.blvalue then
			table.append(outargs, aarg)
		end
	end
	local retti
	if bft.ti.rettype then
		retti = bft.ti.rettype:gettivalue()
	end
	return dinstance:create{
		spos = it.spos,
		epos = it.epos,
		context = it.context,
		args = args,
		outargs = outargs,
		ti = retti,
	}
end

dinstance = require('exl.node.expr.defcall.instance')
