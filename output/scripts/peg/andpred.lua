local modname = ...
local rule = require('peg.rule')
local andpred = rule:module(modname)

andpred.name = '(andpred)'

function andpred:init(it)
	self.rule = it[1]
	rule.init(self, it)
end

function andpred:process(stream, row, col)
	local er, ec, r = self.rule:invoke(stream, row, col)
	if er then
		return row, col, false
	else
		return nil
	end
end

function andpred:wrapresult(row, col, pr)
	return pr
end

function andpred:defstring()
	local str = '&( ' .. tostring(self.rule) .. ' )'
	return str
end
