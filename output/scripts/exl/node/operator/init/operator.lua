local modname = ...
local opbase = require(modname, 2, 'base.operator')
local opinit = opbase:create{
	filename = 'system',
}
package.modtable(modname, opinit)
local invinit

function opinit:invoke(it)
	if #it.args ~= 1 and #it.args ~= 2 then
		return
	end
	return invinit:create{
		spos = it.spos,
		epos = it.epos,
		filename = it.filename,
		context = it.context,
		args = it.args,
	}
end

invinit = require(modname, 1, 'invocation')
