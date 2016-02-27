local modname = ...
local efunctionbase = require(modname, 0, 'base')
local efunction = efunctionbase:module(modname)
local common

function efunction:dobuild(pc)
	efunctionbase.dobuild(self, pc)
	self.body:build(self.context)
end

function efunction:defstring(lp)
	local argstr = {}
	for i, arg in ipairs(self.args) do
		argstr[i] = arg:defstring(lp .. self.lpindent)
	end
	if self.rettype then
		return string.format('function(%s): %s%s\n%send',
			table.concat(argstr, ', '),
			self.rettype:defstring(lp .. self.lpindent),
			self.body:defstring(lp .. self.lpindent),
			lp)
	else
		return string.format('function(%s)%s\n%send',
			table.concat(argstr, ', '),
			self.body:defstring(lp .. self.lpindent),
			lp)
	end
end

common = require(modname, 3, 'common')
