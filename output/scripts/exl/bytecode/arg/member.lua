local modname = ...
local basearg = package.relrequire(modname, 1, 'base')
local memberarg = basearg:module(modname)
local common
local scalars = require('rs.scalars')

function memberarg:init(it)
	basearg.init(self, it)
	self.base = basearg:createarg(it[2])
	self.index = it[3]
end

function memberarg:write(stream)
	self.base:writearg(stream)
	scalars.string:write(self.index, stream)
end

function memberarg:read(stream)
	local instance = self:new()
	instance.base = basearg:readarg(stream)
	instance.index = scalars.string:read(stream)
	return instance
end

function memberarg:defstring(lp)
	return string.format('%s.%s',
		self.base:defstring(lp .. self.lpindent),
		common.identstring(self.index))
end

common = package.relrequire(modname, 3, 'common')
