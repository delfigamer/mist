local modname = ...
local block = require('exl.node.block')
local classbody = block:module(modname)
local common

function classbody:init(pr)
	block.init(self, pr)
end

function classbody:build(pc)
	for i, stat in ipairs(self.statements) do
		stat:build(pc)
	end
end

function classbody:compile(stream)
	-- for i, stat in ipairs(self.statements) do
		-- stream:writetoken('d_filepos', stat.spos.row, stat.spos.col)
		-- stat:compile(stream)
	-- end
end

function classbody:defstring(lp)
	local statlines = {}
	for i, stat in ipairs(self.statements) do
		statlines[i] = ('\n' .. lp) .. common.defstring(stat, lp)
	end
	return table.concat(statlines)
end

common = require('exl.common')
