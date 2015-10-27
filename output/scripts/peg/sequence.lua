local modname = ...
local rule = require('peg.rule')
local cycleresult
local sequence = rule:module(modname)

sequence.name = '(sequence)'

function sequence:init(it)
	self.rules = {}
	for i, rule in ipairs(it) do
		self.rules[i] = rule
	end
	rule.init(self, it)
end

function sequence:process(stream, row, col)
	local result = {}
	for i, rule in ipairs(self.rules) do
		row, col, result[i] = rule:invoke(stream, row, col)
		if not row then
			return nil
		end
	end
	return row, col, result
end

function sequence:wrapresult(row, col, pr)
	return cycleresult:create(self, row, col, pr)
end

function sequence:defstring()
	local lines = {}
	for i, rule in ipairs(self.rules) do
		lines[i] = tostring(rule)
	end
	local str = '( ' .. table.concat(lines, ' ') .. ' )'
	return str
end

cycleresult = require('peg.cycleresult')
