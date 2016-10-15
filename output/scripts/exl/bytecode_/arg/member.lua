local modname = ...
local basearg = require(modname, 1, 'base')
local memberarg = basearg:module(modname)
local common
local scalars = require('rs.scalars')

memberarg.type = 'member'

function memberarg:init(it)
	basearg.init(self, it)
	self.base = basearg:createarg(it[2])
	if type(it[3]) ~= 'string' then
		error('string expected')
	end
	self.index = it[3]
end

function memberarg:write(stream)
	self.base:writearg(stream)
	scalars.string:write(self.index, stream)
end

function memberarg:read(stream)
	self.base = basearg:readarg(stream)
	self.index = scalars.string:read(stream)
end

function memberarg:defstring(lp)
	return string.format('%s.%s',
		self.base:defstring(lp .. self.lpindent),
		common.identstring(self.index))
end

common = require(modname, 3, 'common')
