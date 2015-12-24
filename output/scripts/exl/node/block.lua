local modname = ...
local node = package.relrequire(modname, 1, 'base')
local block = node:module(modname)
local common
local context

function block:init(pr)
	node.init(self, pr)
	self.statements = pr.statements
end

function block:build(pc)
	self.context = context:create(pc)
	for i, stat in ipairs(self.statements) do
		stat:build(self.context)
	end
end

function block:compile(stream)
	for i, stat in ipairs(self.statements) do
		stream:writetoken('d_filepos', stat.spos.row, stat.spos.col)
		stat:compile(stream)
	end
end

function block:defstring(lp)
	local statlines = {}
	for i, stat in ipairs(self.statements) do
		statlines[i] = ('\n' .. lp) .. stat:defstring(lp)
	end
	return table.concat(statlines)
end

common = package.relrequire(modname, 2, 'common')
context = package.relrequire(modname, 2, 'context')
