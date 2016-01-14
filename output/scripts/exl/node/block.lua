local modname = ...
local node = package.relrequire(modname, 1, 'base')
local block = node:module(modname)
local common
local context

function block:init(pr)
	node.init(self, pr)
	self.statements = pr.statements
end

function block:dobuild(pc)
	self.context = context:create(pc)
	for i, stat in ipairs(self.statements) do
		stat:build(self.context)
	end
end

function block:compile(block)
	for i, stat in ipairs(self.statements) do
		block:writetoken{
			op = 'ancillary',
			args = {
				{'string', 'comment'}, -- name
				{'int', stat.spos.row}, -- row
				{'int', stat.spos.col}, -- col
				{'string', self.filename or '-'}, -- filename
				{'string', ''} -- text
			},
		}
		stat:compile(block)
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
