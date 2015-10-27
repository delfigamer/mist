local modname = ...
local proxy = require('peg.proxy')
local concat = proxy:module(modname)

concat.name = '(namemod)'

function concat:wrapresult(row, col, pr)
	return table.concat(pr)
end

function concat:defstring()
	local str = '$concat ' .. self.name .. ' ' .. tostring(self.rule)
	return str
end
