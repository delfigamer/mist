local modname = ...
local opbase = package.relrequire(modname, 2, 'base.operator')
local opassign = opbase:create{
	filename = 'system',
}
package.modtable(modname, opassign)
local invassign

function opassign:invoke(it)
	if #it.args ~= 2 then
		return
	end
	local lv = it.args[1]
	local rv = it.args[2]
	local lft = lv:getfulltype()
	local rft = rv:getfulltype()
	if not lft.lvalue or not rft.rvalue or not lft.ti:iseq(rft.ti) then
		return
	end
	return invassign:create{
		spos = it.spos,
		epos = it.epos,
		filename = it.filename,
		operator = self,
		context = it.context,
		args = it.args,
		ti = lft.ti,
	}
end

invassign = package.relrequire(modname, 1, 'invocation')
