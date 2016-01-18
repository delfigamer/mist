local modname = ...
local opbase = package.relrequire(modname, 2, 'base.operator')
local opprototyped = opbase:module(modname)
-- local invprototyped

function opprototyped:init(pr)
	opbase.init(self, pr)
	self.args = pr.args
	self.retfulltype = pr.retfulltype
end

function opprototyped:invoke(it)
	if #it.args ~= #self.args then
		return
	end
	for i, aarg in ipairs(self.args) do
		local barg = it.args[i]:getfulltype()
		if
			(aarg.lvalue and not barg.lvalue) or
			(aarg.rvalue and not barg.rvalue) or
			not aarg.ti:iseq(barg.ti)
		then
			return
		end
	end
	return self.invocationclass:create{
		context = it.context,
		spos = it.spos,
		epos = it.epos,
		filename = it.filename,
		operator = self,
		args = it.args,
		fulltype = self.retfulltype,
	}
end

-- function opprototyped:defstring(lp)
-- end

opprototyped.invocationclass = package.relrequire(modname, 1, 'invocation')
