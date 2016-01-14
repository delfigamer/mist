local modname = ...
local baseof = package.relrequire(modname, 2, 'base.factory')
local initof = baseof:module(modname)
local common
local initoi

function initof:init(pr)
	baseof.init(self, pr)
end

function initof:createinstance(it)
	if #it.args == 1 then
		return initoi:create{
			spos = it.args[1].spos,
			epos = it.args[1].epos,
			filename = it.args[1].filename,
			context = it.context,
			args = it.args,
		}
	elseif #it.args == 2 then
		local node = common.createnode{
			name = 'expr.operator',
			operator = 'assign',
			spos = it.args[1].spos,
			epos = it.args[2].epos,
			filename = it.args[1].filename,
			args = it.args,
		}
		node:build(it.context)
		return node
	end
end

common = package.relrequire(modname, 4, 'common')
initoi = package.relrequire(modname, 1, 'instance')
