local modname = ...
local rule = require('peg.rule')
local terminal = rule:module(modname)

terminal.name = '(terminal)'

function terminal:init(it)
	local predparam = it[1]
	local predfunc
	if type(predparam) == 'function' then
		predfunc = predparam
	elseif type(predparam) == 'string' then
		predfunc = function(ch)
			return ch and ch == predparam
		end
		self.name = string.format('%q', predparam)
	elseif type(predparam) == 'table' then
		predfunc = function(ch)
			return ch and predparam[ch]
		end
	end
	if it.transform then
		self.pred = predfunc
	else
		self.pred = function(ch)
			return predfunc(ch) and ch or nil
		end
	end
	rule.init(self, it)
end

function terminal:process(stream, row, col)
	if stream.lines[row] then
		if col <= stream.linelen[row] then
			local result = self.pred(stream.lines[row]:sub(col, col))
			if result then
				return row, col + 1, result
			else
				return nil
			end
		else
			local result = self.pred('\n')
			if result then
				return row + 1, 1, result
			else
				return nil
			end
		end
	else
		local result = self.pred(nil)
		if result then
			return row, col, result
		else
			return nil
		end
	end
end
