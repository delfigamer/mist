local modname = ...
local opbase = require(modname, 2, 'base.operator')
local opprototyped = opbase:module(modname)
local ebase
local ecast

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
	local maxrank = 0
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
		if arank > maxrank then
			maxrank = arank
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
		maxrank = maxrank,
	}
end

opprototyped.invocationclass = require(modname, 1, 'invocation')
ebase = require(modname, 3, 'expr.base')
ecast = require(modname, 3, 'expr.cast')
