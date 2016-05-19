local modname = ...
local rule = require('peg.rule')
local sequence = require('peg.sequence')
local structureresult
local structure = sequence:module(modname)

structure.name = '(structure)'
structure.errorscope = true

function structure:init(it)
	self.rules = it.rules
	self.names = it.names or {}
	rule.init(self, it)
end

function structure:wrapresult(row, col, pr)
	return structureresult:create(self, row, col, pr)
end

function structure:defstring()
	local lines = {}
	for i, rule in ipairs(self.rules) do
		lines[i] = tostring(rule)
		if self.names[i] then
			lines[i] = self.names[i] .. ' = ' .. lines[i]
		end
	end
	local str = self.name .. '{ ' ..
		table.concat(lines, ', ') .. ' }'
	return str
end

structureresult = require('peg.structureresult')
