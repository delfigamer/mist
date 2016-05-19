local modname = ...
local rule = require('peg.rule')
local marker = rule:module(modname)

marker.name = '$marker'

function marker:init(it)
	self.str = it[1]
	rule.init(self, it)
end

function marker:process(stream, row, col)
	print('$marker', self.str, row, col)
	return row, col, false
end
