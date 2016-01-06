local modname = ...
local node = package.relrequire(modname, 1, 'base')
local stream = node:module(modname)
local common
local context

function stream:init(pr)
	node.init(self, pr)
	self.statements = pr.statements
end

function stream:build(pc)
	self.context = context:create(pc)
	for i, stat in ipairs(self.statements) do
		stat:build(self.context)
	end
end

function stream:compile(stream)
	for i, stat in ipairs(self.statements) do
		stream:writetoken{
			op = 'ancillary',
			args = {
				{'string', 'comment'}, -- name
				{'int', stat.spos.row}, -- row
				{'int', stat.spos.col}, -- col
				{'string', self.filename or '-'}, -- filename
				{'string', ''} -- text
			},
		}
		stat:compile(stream)
	end
end

function stream:defstring(lp)
	local statlines = {}
	for i, stat in ipairs(self.statements) do
		statlines[i] = ('\n' .. lp) .. stat:defstring(lp)
	end
	return table.concat(statlines)
end

common = package.relrequire(modname, 2, 'common')
context = package.relrequire(modname, 2, 'context')
