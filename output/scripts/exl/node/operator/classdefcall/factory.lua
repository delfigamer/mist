local modname = ...
local baseof = package.relrequire(modname, 2, 'base.factory')
local classcallof = baseof:module(modname)
local classcalloi

function classcallof:init(pr)
	baseof.init(self, pr)
end

function classcallof:createinstance(it)
	local args = it.args
	if #args ~= 1 then
		return
	end
	local base = args[1]
	local bft = base:getfulltype()
	local retti = bft.ti.classinfo:getinstanceti()
	return classcalloi:create{
		spos = it.spos,
		epos = it.epos,
		filename = it.filename,
		context = it.context,
		base = base,
		ti = retti,
	}
end

classcalloi = package.relrequire(modname, 1, 'instance')
