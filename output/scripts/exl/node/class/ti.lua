local modname = ...
local baseti = package.relrequire(modname, 2, 'expr.base.ti')
local classti = baseti:module(modname)
local common

function classti:init(pr)
	baseti.init(self, pr)
	self.parent = pr.parent
	self.body = pr.body
end

function classti:iseq(other)
	if not other['#' .. classti._NAME] then
		return false
	end
	return true
end

function classti:internalresolve(op, proto)
	-- if op == 'call' then
		-- return cofunc
	-- else
		return baseti.internalresolve(self, op, proto)
	-- end
end

function classti:defstring(lp)
	if self.parent then
		return string.format('type class: %s%s\n%send',
			self.parent:defstring(lp .. self.lpindent),
			self.body:defstring(lp .. self.lpindent),
			lp)
	else
		return string.format('type class%s\n%send',
			self.body:defstring(lp .. self.lpindent),
			lp)
	end
end

common = package.relrequire(modname, 3, 'common')
