local modname = ...
local object = require('base.object')
local binaryresult = object:module(modname)

function binaryresult:init(rule, row, col, prefix, infix, suffix)
	self.prefix = prefix
	self.infix = infix
	self.suffix = suffix
	self.row = row
	self.col = col
	self.rule = rule
	self.name = rule.name
end

local function elemstring(elem, lp)
	if type(elem) == 'string' then
		return string.format('%q', elem)
	elseif type(elem) == 'table' and elem.defstring then
		return elem:defstring(lp)
	else
		return tostring(elem)
	end
end

local format_full = [[{name = %q,
%sprefix = %s,
%sinfix = %s,
%ssuffix = %s}]]
local format_short = [[{name = %q,
%sprefix = %s,
%ssuffix = %s}]]

function binaryresult:defstring(lp)
	lp = lp .. '  '
	if self.rule.infix then
		return string.format(format_full, self.name,
			lp, elemstring(self.prefix, lp),
			lp, elemstring(self.infix, lp),
			lp, elemstring(self.suffix, lp))
	else
		return string.format(format_short, self.name,
			lp, elemstring(self.prefix, lp),
			lp, elemstring(self.suffix, lp))
	end
end

function binaryresult.instmeta:__tostring()
	return self:defstring('')
end
