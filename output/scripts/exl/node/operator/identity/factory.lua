local modname = ...
local baseof = package.relrequire(modname, 2, 'base.factory')
local identityof = baseof:module(modname)
local identityoi

function identityof:init(pr)
	baseof.init(self, pr)
end

function identityof:createinstance(it)
	if #it.args ~= 1 then
		return
	end
	local value = it.args[1]
	local vft = value:getfulltype()
	if not vft.rvalue then
		return
	end
	return identityoi:create{
		spos = it.spos,
		epos = it.epos,
		filename = it.filename,
		context = it.context,
		args = it.args,
		ti = vft.ti,
	}
end

identityoi = package.relrequire(modname, 1, 'instance')
