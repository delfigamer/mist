local modname = ...
local opbase = require(modname, 2, 'base.operator')
local opfunctioncall = opbase:create{
	filename = 'system',
}
package.modtable(modname, opfunctioncall)
local invfunctioncall

function opfunctioncall:invoke(it)
	local base = it.args[1]
	local bft = base:getfulltype()
	if not bft.ti['#' .. package.relpath(modname, 3, 'expr.function.ti')] then
		return
	end
	if #bft.ti.args ~= #it.args - 1 then
		return
	end
	local outargs = {}
	local inargs = {}
	for i = 1, #it.args - 1 do
		local aarg = it.args[i+1]
		local farg = bft.ti.args[i]
		local aargft = aarg:getfulltype()
		local fargti = farg.typev:gettivalue()
		if not aargft.lvalue and farg.blvalue then
			return
		elseif not aargft.rvalue and farg.brvalue then
			return
		elseif not aargft.ti:iseq(fargti) then
			return
		end
		if farg.blvalue then
			table.append(outargs, aarg)
		end
		if farg.brvalue then
			table.append(inargs, aarg)
		end
	end
	local retti
	if bft.ti.rettype then
		retti = bft.ti.rettype:gettivalue()
	end
	return invfunctioncall:create{
		spos = it.spos,
		epos = it.epos,
		filename = it.filename,
		operator = self,
		context = it.context,
		base = base,
		outargs = outargs,
		inargs = inargs,
		ti = retti,
	}
end

invfunctioncall = require(modname, 1, 'invocation')
