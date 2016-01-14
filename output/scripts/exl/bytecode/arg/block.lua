local modname = ...
local basearg = package.relrequire(modname, 1, 'base')
local blockarg = basearg:module(modname)
local block
local scalars = require('rs.scalars')

function blockarg:init(it)
	basearg.init(self, it)
	if type(it[2]) ~= 'table' or not it[2]['#'..block._NAME] then
		error('body expected')
	end
	self.block = it[2]
end

function blockarg:write(stream)
	self.block:write(stream)
end

function blockarg:read(stream)
	local block = block:read(stream)
	return blockarg:create{'block', block}
end

function blockarg:defstring(lp)
	return string.format('{\n%s%s}',
		self.block:defstring(lp .. self.lpindent), lp)
end

block = package.relrequire(modname, 2, 'block')
