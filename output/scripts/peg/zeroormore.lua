local modname = ...
local rule = require('peg.rule')
local cycleresult
local zeroormore = rule:module(modname)

zeroormore.name = '(zeroormore)'

function zeroormore:init(it)
	self.rule = it[1]
	rule.init(self, it)
end

function zeroormore:process(stream, row, col)
	local result = {}
	local i = 1
	while true do
		local er, ec, r = self.rule:invoke(stream, row, col)
		if not er then
			break
		end
		row, col, result[i] = er, ec, r
		i = i + 1
	end
	return row, col, result
end

function zeroormore:wrapresult(row, col, pr)
	return cycleresult:create(self, row, col, pr)
end

function zeroormore:defstring()
	local str = '*( ' .. tostring(self.rule) .. ' )'
	return str
end

cycleresult = require('peg.cycleresult')
