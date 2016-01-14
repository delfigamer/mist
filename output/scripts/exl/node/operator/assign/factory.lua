local modname = ...
local baseof = package.relrequire(modname, 2, 'base.factory')
local defassignof = baseof:module(modname)
local defassignoi

function defassignof:init(pr)
	baseof.init(self, pr)
end

function defassignof:createinstance(it)
	if #it.args ~= 2 then
		return
	end
	local lv = it.args[1]
	local rv = it.args[2]
	local lft = lv:getfulltype()
	local rft = rv:getfulltype()
	if not lft.lvalue or not lft.ti:iseq(rft.ti) then
		return
	end
	local spos = it.args[1].spos
	local epos = it.args[2].epos
	return defassignoi:create{
		spos = spos,
		epos = epos,
		filename = it.args[1].filename,
		context = it.context,
		args = it.args,
		ti = lft.ti,
	}
end

defassignoi = package.relrequire(modname, 1, 'instance')
