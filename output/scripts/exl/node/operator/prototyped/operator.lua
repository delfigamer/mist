local modname = ...
local opbase = package.relrequire(modname, 2, 'base.operator')
local opprototyped = opbase:module(modname)
local ebase
local ecast

opprototyped.rank = 0

function opprototyped:init(pr)
	opbase.init(self, pr)
	self.args = pr.args
	self.retfulltype = pr.retfulltype or ebase:getfulltype()
end

function opprototyped:invoke(it)
	if #it.args ~= #self.args then
		return
	end
	local castargs = {}
	local rank = 0
	for i, aarg in ipairs(self.args) do
		local barg = it.args[i]:getfulltype()
		local arank
		castargs[i], arank = ecast:castvalue{
			context = it.context,
			spos = it.spos,
			epos = it.epos,
			filename = it.filename,
			base = it.args[i],
			target = aarg,
			binternal = it.binternal,
		}
		if not castargs[i] then
			return
		end
		if arank > rank then
			rank = arank
		end
	end
	return self.invocationclass:create{
		context = it.context,
		spos = it.spos,
		epos = it.epos,
		filename = it.filename,
		operator = self,
		args = castargs,
		fulltype = self.retfulltype,
		rank = self.rank + rank,
	}
end

opprototyped.invocationclass = package.relrequire(modname, 1, 'invocation')
ebase = package.relrequire(modname, 3, 'expr.base')
ecast = package.relrequire(modname, 3, 'expr.cast')
