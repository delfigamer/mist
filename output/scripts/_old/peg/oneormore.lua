local modname = ...
local zeroormore = require('peg.zeroormore')
local oneormore = zeroormore:module(modname)

oneormore.name = '(oneormore)'

function oneormore:process(stream, row, col)
	local result
	row, col, result = zeroormore.process(self, stream, row, col)
	if result[1] ~= nil then
		return row, col, result
	else
		return nil
	end
end

function oneormore:defstring()
	local str = '%( ' .. tostring(self.rule) .. ' )'
	return str
end
