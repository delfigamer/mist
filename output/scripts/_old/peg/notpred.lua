local modname = ...
local rule = require('peg.rule')
local notpred = rule:module(modname)

notpred.name = '(notpred)'

function notpred:init(it)
	self.rule = it[1]
	rule.init(self, it)
end

function notpred:process(stream, row, col)
	local er, ec, r = self.rule:invoke(stream, row, col)
	if er then
		return nil
	else
		return row, col, false
	end
end

function notpred:wrapresult(row, col, pr)
	return pr
end

function notpred:defstring()
	local str = '~( ' .. tostring(self.rule) .. ' )'
	return str
end
