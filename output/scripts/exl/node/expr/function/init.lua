local modname = ...
local efunctionbase = package.relrequire(modname, 0, 'base')
local efunction = efunctionbase:module(modname)
local common

function efunction:build(pc)
	efunctionbase.build(self, pc)
	self.body:build(self.context)
end

function efunction:defstring(lp)
	if self.rettype then
		return string.format('function%s: %s%s\n%send',
			self.arglist:defstring(lp .. self.lpindent),
			self.rettype:defstring(lp .. self.lpindent),
			self.body:defstring(lp .. self.lpindent),
			lp)
	else
		return string.format('function%s%s\n%send',
			self.arglist:defstring(lp .. self.lpindent),
			self.body:defstring(lp .. self.lpindent),
			lp)
	end
end

common = package.relrequire(modname, 3, 'common')
