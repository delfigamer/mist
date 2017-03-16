local modname = ...
local opbase = require(modname, 2, 'base.operator')
local opclasscall = opbase:create{
	filename = 'system',
}
package.modtable(modname, opclasscall)
local invclasscall

function opclasscall:init(pr)
	opbase.init(self, pr)
end

function opclasscall:invoke(it)
	local args = it.args
	if #args ~= 1 then
		return
	end
	local base = args[1]
	local bft = base:getfulltype()
	local retti = bft.ti.classinfo:getinstanceti()
	return invclasscall:create{
		spos = it.spos,
		epos = it.epos,
		filename = it.filename,
		context = it.context,
		base = base,
		ti = retti,
	}
end

invclasscall = require(modname, 1, 'invocation')
