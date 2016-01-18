local modname = ...
local baseof = package.relrequire(modname, 2, 'base.factory')
local defcallof = baseof:module(modname)
local defcalloi

function defcallof:init(pr)
	baseof.init(self, pr)
end

function defcallof:createinstance(it)
	local args = it.args
	local base = args[1]
	local bft = base:getfulltype()
	if #bft.ti.args ~= #args - 1 then
		return
	end
	local outargs = {}
	local inargs = {}
	for i = 1, #args - 1 do
		local aarg = args[i+1]
		local farg = bft.ti.args[i]
		local aargft = aarg:getfulltype()
		local fargti = farg.typev:gettivalue()
		if not aargft or not fargti then
			return
		elseif not aargft.lvalue and farg.blvalue then
			return
		elseif not aargft.rvalue and farg.brvalue then
			return
		elseif not aargft.ti or not aargft.ti:iseq(fargti) then
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
	return defcalloi:create{
		spos = it.spos,
		epos = it.epos,
		filename = it.filename,
		context = it.context,
		base = base,
		outargs = outargs,
		inargs = inargs,
		ti = retti,
	}
end

defcalloi = package.relrequire(modname, 1, 'instance')
