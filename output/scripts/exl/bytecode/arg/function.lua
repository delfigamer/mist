local modname = ...
local basearg = package.relrequire(modname, 1, 'base')
local functionarg = basearg:module(modname)
local block
local scalars = require('rs.scalars')

functionarg.type = 'function'

function functionarg:init(it)
	basearg.init(self, it)
	if type(it[2]) ~= 'table' then
		error('arglist expected')
	end
	self.args = it[2]
	if type(it[3]) ~= 'table' or not it[3]['#'..block._NAME] then
		error('body expected')
	end
	self.body = it[3]
end

function functionarg:write(stream)
	scalars.int:write(#self.args, stream)
	for i = 1, #self.args do
		scalars.string:write(self.args[i], stream)
	end
	self.body:write(stream)
end

function functionarg:read(stream)
	local argc = scalars.int:read(stream)
	self.args = {}
	for i = 1, argc do
		self.args[i] = scalars.string:read(stream)
	end
	self.body = block:read(stream)
end

function functionarg:defstring(lp)
	return string.format('.function[%s] {\n%s%s}',
		table.concat(self.args, ', '),
		self.body:defstring(lp .. self.lpindent), lp)
end

block = package.relrequire(modname, 2, 'block')
