local modname = ...
local rule = require('peg.rule')
local constant = rule:module(modname)

constant.name = '(constant)'

local cache = setmetatable({}, {__mode='v'})

function constant:create(it)
	local str = it[1] or ''
	if cache[str] then
		return cache[str]
	else
		return rule.create(self, it)
	end
end

function constant:init(it)
	self.value = it[1] or ''
	rule.init(self, it)
	if not it.name then
		self.name = '\'' .. self.value:gsub('\'', '\'\'') .. '\''
	end
end

function constant:process(stream, row, col)
	local line = stream.lines[row]
	if not line then
		return nil
	end
	line = line:sub(col, col + #self.value - 1)
	if line == self.value then
		return row, col + #self.value, self.value
	else
		return nil
	end
end
