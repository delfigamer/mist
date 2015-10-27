local modname = ...
local rule = require('peg.rule')
local dummy = rule:module(modname)

dummy.name = '$dummy'

function dummy:init(it)
	rule.init(self, it)
end

function dummy:process(stream, row, col)
	return row, col, false
end
