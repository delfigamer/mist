local modname = ...
module(modname, package.seeall)
local peg = require('peg')
local int = require('ppcasm.int')
local instructions = require('ppcasm.instructions')

syntax = peg.loadfile(_PATH .. 'scripts/ppcasm/syntax.peg')

function parse(str)
	return select(3, syntax:invoke(peg.stream:create(str)))
end

function translate(pr, labels)
	local func = instructions[pr.name]
	return func(pr, labels)
end

function assemble(str, offset)
	offset = offset or 0
	local pr = parse(str)
	local labels = {}
	for i, line in ipairs(pr._lines) do
		if line._label then
			labels[line._label] = offset
		end
		if line._instruction then
			line._instruction.address = offset
			offset = offset + 4
		end
	end
	local output = {}
	local outputlen = 0
	for i, line in ipairs(pr._lines) do
		if line._instruction then
			outputlen = outputlen + 1
			local suc, result = pcall(translate, line._instruction, labels)
			if suc then
				output[outputlen] = int.be32(result)
			else
				result.message = string.format('line %i: %s', line.row, result.message)
				error(result)
			end
		end
	end
	return table.concat(output)
end
