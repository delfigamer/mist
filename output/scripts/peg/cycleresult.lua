local modname = ...
local object = require('base.object')
local cycleresult = object:module(modname)

cycleresult.issequence = true

function cycleresult:init(rule, row, col, pr)
	for i, elem in ipairs(pr) do
		self[i] = elem
	end
	self.row = row
	self.col = col
	self.rule = rule
end

function cycleresult:defstring(lp)
	lp = lp..'  '
	local lines = {}
	for i, elem in ipairs(self) do
		if type(elem) == 'string' then
			elem = string.format('%q', elem)
		elseif type(elem) == 'table' and elem.defstring then
			elem = elem:defstring(lp)
		else
			elem = tostring(elem)
		end
		lines[i] = string.format('%s[%i] = %s', lp, i, elem)
	end
	if #lines > 0 then
		return string.format('{\n%s}', table.concat(lines, ',\n'))
	else
		return '{}'
	end
end

function cycleresult.instmeta:__tostring()
	return self:defstring('')
end
