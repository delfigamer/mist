local modname = ...
local rule = require('peg.rule')
local choice = rule:module(modname)

choice.name = '(choice)'

function choice:init(it)
	self.rules = {}
	for i, rule in ipairs(it) do
		self.rules[i] = rule
	end
	rule.init(self, it)
end

function choice:process(stream, row, col)
	for i, rule in ipairs(self.rules) do
		local er, ec, result = rule:invoke(stream, row, col)
		if er then
			return er, ec, result
		end
	end
	return nil
end

function choice:defstring()
	local lines = {}
	for i, rule in ipairs(self.rules) do
		lines[i] = tostring(rule)
	end
	local str = '( ' .. table.concat(lines, ' / ') .. ' )'
	return str
end
