local modname = ...
local rule = require('peg.rule')
local frame = rule:module(modname)

frame.name = '(frame)'

function frame:init(it)
	self.left = it.left
	self.center = it.center
	self.right = it.right
	rule.init(self, it)
end

function frame:process(stream, row, col)
	local result, tr
	if self.left then
		row, col, tr = self.left:invoke(stream, row, col)
		if not row then
			return nil
		end
	end
	row, col, result = self.center:invoke(stream, row, col)
	if not row then
		return nil
	end
	if self.right then
		row, col, tr = self.right:invoke(stream, row, col)
		if not row then
			return nil
		end
	end
	return row, col, result
end

function frame:defstring()
	local str = '( '
	if self.left then
		str = str .. tostring(self.left) .. ' < '
	end
	str = str .. tostring(self.center)
	if self.right then
		str = str .. ' > ' .. tostring(self.right)
	end
	str = str .. ' )'
	return str
end
