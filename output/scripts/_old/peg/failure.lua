local modname = ...
local rule = require('peg.rule')
local failure = rule:module(modname)

failure.name = '$error'
failure.errorscope = true

function failure:init(it)
	self.msg = it[1]
	rule.init(self, it)
end

function failure:process(stream, row, col)
	error(self.msg, 0)
end

function failure:defstring()
	local str = '$error \'' ..
		tostring(self.msg):gsub('\'', '\'\'') .. '\''
	return str
end
