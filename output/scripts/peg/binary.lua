local modname = ...
local rule = require('peg.rule')
local binaryresult
local binary = rule:module(modname)

binary.name = '(binary)'

function binary:init(it)
	self.prefix = it.prefix
	self.infix = it.infix
	self.suffix = it.suffix
	rule.init(self, it)
end

function binary:process(stream, row, col)
	local prefix
	local infixes = {}
	local suffixes = {}
	row, col, prefix = self.prefix:invoke(stream, row, col)
	if not row then
		return nil
	end
	local i = 1
	while true do
		local irow, icol, ires
		if self.infix then
			irow, icol, ires = self.infix:invoke(stream, row, col)
			if not irow then
				break
			end
		else
			irow, icol = row, col
		end
		local srow, scol, sres = self.suffix:invoke(stream, irow, icol)
		if not srow then
			break
		end
		row, col, infixes[i], suffixes[i] = srow, scol, ires, sres
		i = i + 1
	end
	return row, col, {
		prefix = prefix,
		infixes = infixes,
		suffixes = suffixes}
end

function binary:wrapresult(row, col, pr)
	local current = pr.prefix
	for i, suffix in ipairs(pr.suffixes) do
		current = binaryresult:create(
			self, row, col, current, pr.infixes[i], suffix)
	end
	return current
end

function binary:defstring()
	local str = '$binary ' .. self.name .. '{' .. tostring(self.prefix) .. ', '
	if self.infix then
		str = str .. tostring(self.prefix) .. ', '
	end
	str = str .. tostring(self.prefix) .. '}'
	return str
end

binaryresult = require('peg.binaryresult')
