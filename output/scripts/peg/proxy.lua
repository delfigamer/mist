local modname = ...
local rule = require('peg.rule')
local proxy = rule:module(modname)

proxy.name = '(proxy)'

function proxy:init(it)
	self.rule = it[1]
	rule.init(self, it)
end

function proxy:process(stream, row, col)
	return self.rule:invoke(stream, row, col)
end

function proxy:wrapresult(row, col, pr)
	return pr
end

function proxy:defstring()
	local str = '( ' .. tostring(self.rule) .. ' )'
	return str
end
