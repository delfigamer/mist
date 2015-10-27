local modname = ...
local object = require('base.object')
local structureresult = object:module(modname)

function structureresult:init(rule, row, col, pr)
	for i, elem in ipairs(pr) do
		self[i] = elem
		local name = rule.names[i]
		if name then
			self['_' .. name] = elem
		end
	end
	self.row = row
	self.col = col
	self.rule = rule
	self.name = rule.name
end

function structureresult:defstring(lp)
	lp = lp..'  '
	local lines = {}
	local li = 1
	for i, elem in ipairs(self) do
		if type(elem) == 'string' then
			elem = string.format('%q', elem)
		elseif type(elem) == 'table' and elem.defstring then
			elem = elem:defstring(lp)
		else
			elem = tostring(elem)
		end
		name = self.rule.names[i]
		if name then
			lines[li] = string.format('%s_%s = %s', lp, name, elem)
			li = li + 1
		end
	end
	table.sort(lines)
	if #lines > 0 then
		return string.format('{name = %q,\n%s}', self.name,
			table.concat(lines, ',\n'))
	else
		return string.format('{name = %q}', self.name)
	end
end

function structureresult.instmeta:__tostring()
	return self:defstring('')
end
