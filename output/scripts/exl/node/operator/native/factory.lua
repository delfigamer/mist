local modname = ...
local baseof = package.relrequire(modname, 2, 'base.factory')
local nativeof = baseof:module(modname)
local nativeoi

function nativeof:init(pr)
	baseof.init(self, pr)
	self.rettype = pr.rettype
	self.constfunc = pr.constfunc
	self.opcode = pr.opcode
end

function nativeof:createinstance(it)
	local spos, epos, filename
	if it.args[1] then
		spos = it.args[1].spos
		epos = it.args[#it.args].epos
		filename = it.args[1].filename
	end
	return nativeoi:create{
		context = it.context,
		spos = spos,
		epos = epos,
		filename = filename,
		nofunc = self,
		args = it.args,
	}
end

nativeoi = package.relrequire(modname, 1, 'instance')
