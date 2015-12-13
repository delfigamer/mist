local modname = ...
local typeinfo = require('exl.typeinfo')
local classti = typeinfo:module(modname)
local common

function classti:init(pr)
	typeinfo.init(self, pr)
	self.parent = pr.parent
	self.body = pr.body
end

function classti:iseq(other)
	if not other['#' .. classti._NAME] then
		return false
	end
	return true
end

function classti:getdefaultopfunc(op, proto)
	-- if op == 'call' then
		-- return cofunc
	-- else
		return typeinfo.getdefaultopfunc(self, op, proto)
	-- end
end

function classti:defstring(lp)
	if self.parent then
		return string.format('type class: %s%s\n%send',
			common.defstring(self.parent, lp .. self.lpindent),
			common.defstring(self.body, lp .. self.lpindent),
			lp)
	else
		return string.format('type class%s\n%send',
			common.defstring(self.body, lp .. self.lpindent),
			lp)
	end
end

common = require('exl.common')
