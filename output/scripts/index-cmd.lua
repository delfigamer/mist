local modname = ...
local index = package.modtable(modname)
require('utils.allocateconsole')()
local exception = require('base.exception')
local host = require('host')

local interrupt = exception:derive({}, 'index-cmd.interrupt')

local getch
local ungetch
local peekch
local tokenstack = {}
do
	local buffer = {}
	local newline = true

	local function getch_raw()
		if newline then
			if #tokenstack > 0 then
				host.write(' ')
				host.write(table.concat(tokenstack, ' | '))
				host.write(' >> ')
			else
				host.write('>> ')
			end
			newline = false
		end
		local ch = host.getchar()
		if ch == '\n' then
			newline = true
		elseif ch == '\26' then
			interrupt:throw('\\26 interrupt')
		end
		return ch
	end

	function getch()
		local ch
		if #buffer > 0 then
			ch = table.pop(buffer)
		else
			ch = getch_raw()
		end
		return ch
	end

	function ungetch(ch)
		table.append(buffer, ch)
	end

	function peekch()
		local ch = getch()
		ungetch(ch)
		return ch
	end
end

local obtaintoken

local function obtainstringelement()
	local ch = getch()
	if ch == '\\' then
		ch = ch .. getch()
	end
	return ch
end

local function obtainstring()
	local open = getch()
	table.append(tokenstack, open)
	local content = {open}
	while true do
		local element = obtainstringelement()
		table.append(content, element)
		if element == open then
			break
		elseif element == '\n' then
			if peekch() == '\n' then
				table.append(content, getch())
				break
			end
		end
	end
	table.pop(tokenstack)
	return table.concat(content)
end

local function obtainlongstring(level)
	local open = '[' .. string.rep('=', level) .. '['
	local content = {open}
	table.append(tokenstack, open)
	while true do
		local ch = getch()
		table.append(content, ch)
		if ch == ']' then
			for i = 1, level do
				ch = getch()
				if ch == '=' then
					table.append(content, ch)
				else
					ungetch(ch)
					ch = nil
					break
				end
			end
			if ch then
				ch = getch()
				if ch == ']' then
					table.append(content, ch)
					break
				else
					ungetch(ch)
				end
			end
		elseif ch == '\n' then
			if peekch() == '\n' then
				table.append(content, getch())
				break
			end
		end
	end
	table.pop(tokenstack)
	return table.concat(content)
end

local letter = {
	['_'] = true,
}
local letterordigit = {
	['_'] = true,
}
for i = string.byte('A'), string.byte('Z') do
	letter[string.char(i)] = true
	letterordigit[string.char(i)] = true
end
for i = string.byte('a'), string.byte('z') do
	letter[string.char(i)] = true
	letterordigit[string.char(i)] = true
end
for i = string.byte('0'), string.byte('9') do
	letterordigit[string.char(i)] = true
end

local wordtable = {
	['do'] = 'end',
	['function'] = 'end',
	['if'] = 'end',
	['repeat'] = 'until',
}

local function obtainwordbody(open)
	table.append(tokenstack, open)
	local close = wordtable[open]
	local content = {open}
	while true do
		local token = obtaintoken()
		table.append(content, token)
		if token == close then
			break
		elseif token == '\n' then
			if peekch() == '\n' then
				table.append(content, getch())
				break
			end
		end
	end
	table.pop(tokenstack)
	return table.concat(content)
end

local function obtainword()
	local word = ''
	while true do
		local ch = getch()
		if letterordigit[ch] then
			word = word .. ch
		else
			ungetch(ch)
			break
		end
	end
	if wordtable[word] then
		return obtainwordbody(word)
	else
		return word
	end
end

local brackettable = {
	['('] = ')',
	['['] = ']',
	['{'] = '}',
}

local function obtainbracket()
	local open = getch()
	table.append(tokenstack, open)
	local close = brackettable[open]
	local content = {open}
	while true do
		local token = obtaintoken()
		table.append(content, token)
		if token == close then
			break
		elseif token == '\n' then
			if peekch() == '\n' then
				table.append(content, getch())
				break
			end
		end
	end
	table.pop(tokenstack)
	return table.concat(content)
end

local tokentable = {
	['\''] = obtainstring,
	['"'] = obtainstring,
}
for ch in pairs(letter) do
	tokentable[ch] = obtainword
end
for ch in pairs(brackettable) do
	tokentable[ch] = obtainbracket
end

tokentable['['] = function()
	local open = getch()
	local mid = {}
	for i = 1, 16 do
		local ch = getch()
		if ch == '[' then
			return obtainlongstring(#mid)
		end
		table.append(mid, ch)
		if ch ~= '=' then
			break
		end
	end
	for i = #mid, 1, -1 do
		ungetch(mid[i])
	end
	ungetch(open)
	return obtainbracket()
end

function obtaintoken()
	local ch = peekch()
	local tfunc = tokentable[ch]
	if tfunc then
		return tfunc()
	else
		return getch()
	end
end

local function obtaininput()
	local str = ''
	while true do
		local token = obtaintoken()
		if token == '\n' then
			break
		end
		str = str .. token
	end
	return str
end

log('entered interactive mode')
local terminate = false

local function exit()
	terminate = true
end

local fenv = setmetatable({exit = exit}, {__index = _G})
fenv._ENV = fenv
print('Interactive input. Type Ctrl+Z or call "exit()" to exit\n')

while not terminate do
	local suc, str = pcall(obtaininput)
	if not suc then
		if type(str) == 'table' and str['#'..interrupt._NAME] then
			break
		else
			error(str)
		end
	end
	local func, err = loadstring('print(' .. str .. ')', nil, nil, fenv)
	if func then
		local suc, err = pcall(func)
		if not suc then
			print(err)
		end
	else
		func, err = loadstring(str, nil, nil, fenv)
		if func then
			local suc, err = pcall(func)
			if not suc then
				print(err)
			end
		else
			print(err)
		end
	end
end
