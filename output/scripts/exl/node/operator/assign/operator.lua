local modname = ...
local opbase = require(modname, 2, 'base.operator')
local opassign = opbase:create{
	filename = 'system',
}
package.modtable(modname, opassign)
local ecast
local fulltype
local invassign

function opassign:invoke(it)
	if #it.args ~= 2 then
		return
	end
	local lv = it.args[1]
	local rv = it.args[2]
	local lft = lv:getfulltype()
	local rft = rv:getfulltype()
	local rank
	rv, rank = ecast:castvalue{
		context = it.context,
		spos = it.spos,
		epos = it.epos,
		filename = it.filename,
		base = rv,
		target = fulltype:create(lft.ti, false, true),
		binternal = it.binternal,
	}
	if not rv then
		return
	end
	return invassign:create{
		spos = it.spos,
		epos = it.epos,
		filename = it.filename,
		operator = self,
		context = it.context,
		args = {lv, rv},
		ti = lft.ti,
		rank = rank,
	}
end

ecast = require(modname, 3, 'expr.cast')
fulltype = require(modname, 4, 'fulltype')
invassign = require(modname, 1, 'invocation')
