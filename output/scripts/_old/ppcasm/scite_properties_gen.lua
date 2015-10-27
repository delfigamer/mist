local maxlinelen = 80

local function concat_limited(t)
	local lines = {}
	local i = 1
	while true do
		local line = '\t' .. t[i]
		local len = #line + 10
		i = i + 1
		while t[i] and (len + #t[i] + 1) <= maxlinelen do
			line = line .. ' ' .. t[i]
			len = len + #t[i] + 1
			i = i + 1
		end
		if t[i] then
			line = line .. ' \\'
		else
			lines[#lines + 1] = line
			break
		end
		lines[#lines + 1] = line
	end
	return table.concat(lines, '\n')
end

local function combine(t1, t2, t3, ...)
	if not t2 then
		return t1
	end
	if t3 then
		return combine(t1, combine(t2, t3, ...))
	end
	local result = {}
	local rlen = 0
	for i, prefix in ipairs(t1) do
		for j, suffix in ipairs(t2) do
			rlen = rlen + 1
			result[rlen] = prefix .. suffix
		end
	end
	return result
end

local function merge(...)
	local result = {}
	local pos = 0
	for i, t in ipairs({...}) do
		for j, val in ipairs(t) do
			pos = pos + 1
			result[pos] = val
		end
	end
	return result
end

local condition = {'lt', 'le', 'eq', 'ge', 'gt', 'nl', 'ne', 'ng',
	'so', 'ns'}
local logical = {'and', 'or', 'xor', 'nand', 'nor', 'eqv', 'andc', 'orc'}
local registerindex = {}
for i = 1, 32 do
	registerindex[i] = tostring(i)
end
local branch_instruction = merge(
	combine({'b', 'bc'}, {'', 'l'}, {'', 'a'}),
	combine({'bclr', 'bcctr'}, {'', 'l'}),
	combine({'b'}, condition, {'', 'l'}, {'', 'a'}),
	combine({'b'}, condition, {'lr', 'ctr'}, {'', 'l'}),
	{'sc'},
	combine({'cr'}, logical),
	combine({'cr'}, {'move', 'clr', 'not', 'set'}),
	{'mcrf'})
local fixedpoint_instruction = merge(
	combine({'lbz', 'lha', 'lhz', 'lwz', 'ld'}, {'', 'u'}, {'', 'x'}),
	{'lwa', 'lwax', 'lwaux'},
	combine({'stb', 'sth', 'stw', 'std'}, {'', 'u'}, {'', 'x'}),
	combine({'addi', 'subi'}, {'', 's'}),
	combine({'add', 'sub', 'subf'}, {'', 'o'}, {'', '.'}))
local register = merge(
	combine({'r', 'f'}, registerindex))

local result_str = [[
# Define SciTE settings for PPC Assembler files (ppcasm)

# ppcasm files
file.patterns.asm=*.asm
filter.asm=Assembler PowerPC(asm)|$(file.patterns.asm)|

lexer.$(file.patterns.asm)=asm

branch_instruction = \
]] .. concat_limited(branch_instruction) .. [[

fixedpoint_instruction = \
]] .. concat_limited(fixedpoint_instruction) .. [[

register = \
]] .. concat_limited(register) .. [[

directive = \
	.long

keywords.$(file.patterns.asm)=$(branch_instruction) $(fixedpoint_instruction)
keywords2.$(file.patterns.asm)=
keywords3.$(file.patterns.asm)=$(register)
keywords4.$(file.patterns.asm)=$(directive)
keywords5.$(file.patterns.asm)=
keywords6.$(file.patterns.asm)=

comment.block.asm=#

# Assembler Styles
#style.asm.32=fore:#808080,font:Verdana,size:8
# Default
style.asm.0=
# Comment
style.asm.1=fore:#adadad
#$(colour.code.comment.line),font:Comic Sans MS,size:8
# Number
style.asm.2=fore:#ff0000
# String
style.asm.3=$(colour.string)
# Operator
style.asm.4=$(colour.operator),bold
# Identifier
style.asm.5=
# CPU instruction
style.asm.6=$(colour.keyword),bold
#fore:#0000ff
# FPU instruction
style.asm.7=fore:#0000ff
# Register
style.asm.8=fore:#46aa03,bold
# assembler Directive
style.asm.9=fore:#0000ff
# assembler Directive Operand
style.asm.10=fore:#0000ff
# Comment block (GNU as /*...*/ syntax, unimplemented)
style.asm.11=$(colour.code.comment.box),$(font.code.comment.box)
# Character/String (single quote) (also character prefix in GNU as)
style.asm.12=$(colour.char)
# End of line where string is not closed
style.asm.13=fore:#000000,back:#E0C0E0,eolfilled
# Extended instructions
style.asm.14=fore:#B00040

#command.compile.$(file.patterns.asm)=nasm $(FileNameExt)

#command.name.0.*.asm=Link
#command.0.*.asm=link $(FileName)


#statement.indent.$(file.patterns.asm)=9 .if
]]

local outf = assert(io.open('asm.properties', 'w'))
outf:write(result_str)
outf:close()
-- print(result_str)
