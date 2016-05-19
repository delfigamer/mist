local modname = ...
local sequence = require('peg.sequence')
local cycleresult
local flatseq = sequence:module(modname)

flatseq.name = '(flatseq)'

function flatseq:wrapresult(row, col, pr)
	local result = {}
	local ri = 1
	for i, elem in ipairs(pr) do
		if type(elem) == 'table' and elem.issequence then
			for j, subelem in ipairs(elem) do
				result[ri] = subelem
				ri = ri + 1
			end
		elseif type(elem) == 'boolean' then
		else
			result[ri] = elem
			ri = ri + 1
		end
	end
	return cycleresult:create(self, row, col, result)
end

function flatseq:defstring()
	local lines = {}
	for i, rule in ipairs(self.rules) do
		lines[i] = tostring(rule)
	end
	local str = '( ' .. table.concat(lines, ' : ') .. ' )'
	return str
end

cycleresult = require('peg.cycleresult')
