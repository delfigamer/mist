local modname = ...
local rule = require('peg.rule')
local optional = rule:module(modname)

optional.name = '(optional)'

function optional:init(it)
	self.rule = it[1]
	rule.init(self, it)
end

function optional:process(stream, row, col)
	local er, ec, result = self.rule:invoke(stream, row, col)
	if er then
		return er, ec, result
	else
		return row, col, false
	end
end

function optional:wrapresult(row, col, pr)
	return pr
end

function optional:setsolid()
	self.solid = true
	self.rule:setsolid()
end

function optional:defstring()
	local str = '?( ' .. tostring(self.rule) .. ' )'
	return str
end
