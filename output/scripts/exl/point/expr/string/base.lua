local modname = ...
local point = require('exl.point')
local stringbaseexpr = point:module(modname)
local common

function stringbaseexpr:init(pr, context)
	point.init(self, pr, context)
	local elements = {}
	for i, elem in ipairs(pr._content) do
		elements[i] = tostring(common.createpoint(elem))
	end
	self.stringvalue = table.concat(elements)
end

local convtable = {
	['\n'] = '\\n',
	['\r'] = '\\r',
	['\t'] = '\\t',
	['\''] = '\\\'',
	['\\'] = '\\\\',
}
for i = 0, 31 do
	local ch = string.char(i)
	if not convtable[ch] then
		convtable[ch] = string.format('\\x%.2x', i)
	end
end

function stringbaseexpr:defstring(lp)
	return string.format('\'%s\'', self.stringvalue:gsub('[%0\1-\31\'\\]', convtable))
end

common = require('exl.common')
