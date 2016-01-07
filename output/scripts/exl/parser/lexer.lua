local modname = ...
local lexer = package.modtable(modname)
local object = require('base.object')
local token = package.relrequire(modname, 1, 'token')
local utf8 = require('utf8')

-- input stream interface:
-- function stream:getc() - returns '' on eof
-- function stream:ungetc()
-- function stream:getpos() - returns the current position
-- function stream:error(msg, spos, epos) - raises an error with a range pointer

--[[
exl tokens:
	'identifier'
	'number' - contains the value
	'string' - contains the converted payload
	'eof'
	lexer.keywords - each one is a distinct type
	symbols and digraphs - each one is a distinct type

	'newline' - internal token, is not returned by lexer.obtaintoken
	instead, token:islinestart of the next one returns true

comments and blanks are ignored
]]

lexer.keyword = table.makeset{
	'as',
	'cast',
	'class',
	'const',
	'end',
	'function',
	'in',
	'inout',
	'local',
	'nil',
	'operator',
	'out',
	'type',
	'unit',
}
local decdigit = {
	['0'] = 0, ['5'] = 5,
	['1'] = 1, ['6'] = 6,
	['2'] = 2, ['7'] = 7,
	['3'] = 3, ['8'] = 8,
	['4'] = 4, ['9'] = 9,
}
local hexdigit = {
	['0'] = 0, ['8'] = 8,
	['1'] = 1, ['9'] = 9,
	['2'] = 2, ['a'] = 10, ['A'] = 10,
	['3'] = 3, ['b'] = 11, ['B'] = 11,
	['4'] = 4, ['c'] = 12, ['C'] = 12,
	['5'] = 5, ['d'] = 13, ['D'] = 13,
	['6'] = 6, ['e'] = 14, ['E'] = 14,
	['7'] = 7, ['f'] = 15, ['F'] = 15,
}
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
local whitespace = {}
for i = 0, 32 do
	whitespace[string.char(i)] = true
end

local function obtaincharacter(stream, char)
	local c = stream:getc()
	if c == char then
		return true
	else
		stream:ungetc(c)
		stream:error(char..' expected', stream:getpos())
	end
end

local function obtaindec(stream, length)
	local ret = 0
	for i = 1, length do
		local ch = stream:getc()
		if decdigit[ch] then
			ret = ret * 10 + decdigit[ch]
		else
			stream:ungetc(ch)
			break
		end
	end
	return ret
end

local function obtainhexfixed(stream, length)
	local ret = 0
	for i = 1, length do
		local ch = stream:getc()
		if hexdigit[ch] then
			ret = ret * 16 + hexdigit[ch]
		else
			stream:ungetc(ch)
			stream:error('invalid hex digit', stream:getpos())
		end
	end
	return ret
end

local function obtainhex(stream, length)
	local ret = 0
	for i = 1, length do
		local ch = stream:getc()
		if hexdigit[ch] then
			ret = ret * 16 + hexdigit[ch]
		else
			stream:ungetc(ch)
			if i == 1 then
				stream:error('invalid hex digit', stream:getpos())
			else
				break
			end
		end
	end
	return ret
end

local stringelement_escape = {
	['a'] = '\x07',
	['b'] = '\x08',
	['f'] = '\x0c',
	['n'] = '\x0a',
	['r'] = '\x0d',
	['t'] = '\x09',
	['v'] = '\x0b',
	['\\'] = '\\',
	['\"'] = '\"',
	['\''] = '\'',
	['\n'] = '\n',
}

-- returns a converted value
-- first character is given in an argument
local function obtainstringelement(stream, first)
	if first == '\\' then
		local ch = stream:getc()
		local value = stringelement_escape[ch]
		if value then
			return value
		elseif ch == 'x' then
			local val = obtainhexfixed(stream, 2)
			return val and string.char(val) or ''
		elseif decdigit[ch] then
			stream:ungetc(ch)
			local val = obtaindec(stream, 1, 3)
			if val > 255 then
				stream:error('invalid byte value', stream:getpos())
			else
				return string.char(val)
			end
		elseif ch == 'u' then
			if not obtaincharacter(stream, '{') then
				return ''
			end
			local val = obtainhex(stream, 8)
			if not obtaincharacter(stream, '}') then
				return ''
			end
			local r = utf8.encodechar(val)
			if r then
				return r
			else
				stream:error('invalid code point', stream:getpos())
			end
		elseif ch == 'z' then
			repeat
				ch = stream:getc()
			until not whitespace[ch]
			stream:ungetc(ch)
			return ''
		else
			stream:error('unknown escape character', stream:getpos())
		end
	else
		return first
	end
end

local function obtainstring(stream)
	local spos = stream:getpos()
	local open = stream:getc()
	local content = {}
	while true do
		local ch = stream:getc()
		if ch == open then
			break
		elseif ch == '\n' or ch == '' then
			stream:error('unterminated string', spos, stream:getpos())
		end
		table.append(content, obtainstringelement(stream, ch))
	end
	return token:create('string', table.concat(content), spos, stream:getpos())
end

local function obtainlongstring(stream, level, spos)
	local content = {}
	while true do
		local ch = stream:getc()
		if ch == ']' then
			for i = 1, level do
				ch = stream:getc()
				if ch ~= '=' then
					stream:ungetc(ch)
					table.append(content, ']')
					table.append(content, string.rep('=', i-1))
					ch = nil
					break
				end
			end
			if ch then
				ch = stream:getc()
				if ch == ']' then
					break
				else
					stream:ungetc(ch)
					table.append(content, ']')
					table.append(content, string.rep('=', level))
				end
			end
		elseif ch == '' then
			stream:error('unterminated long string', spos, stream:getpos())
		else
			table.append(content, ch)
		end
	end
	return token:create('string', table.concat(content), spos, stream:getpos())
end

local function obtainword(stream)
	local spos = stream:getpos()
	local word = {}
	while true do
		local ch = stream:getc()
		if letterordigit[ch] then
			table.append(word, ch)
		else
			stream:ungetc(ch)
			break
		end
	end
	word = table.concat(word)
	if lexer.keyword[word] then
		return token:create(word, nil, spos, stream:getpos())
	else
		return token:create('identifier', word, spos, stream:getpos())
	end
end

local function skipinvalidnumber(stream)
	local ch
	repeat
		ch = stream:getc()
	until not letterordigit[ch] and ch ~= '.'
	stream:ungetc(ch)
end

local function obtainnumber_base(
		stream, base, digitmap, explower, expupper, expbase, spos)
	local val = 0
	local ch = stream:getc()
	local intpart
	local fracpart
	if digitmap[ch] then
		intpart = true
	end
	while true do
		if digitmap[ch] then
			val = val * base + digitmap[ch]
		elseif ch == '.' then
			goto fraction
		elseif ch == explower or ch == exphigher then
			goto exponent
		elseif letterordigit[ch] then
			goto fail
		else
			stream:ungetc(ch)
			goto success
		end
		ch = stream:getc()
	end
::fraction::
	do
		ch = stream:getc()
		if digitmap[ch] then
			fracpart = true
		end
		if not intpart and not fracpart then
			goto fail
		end
		local factor = 1/base
		while true do
			if digitmap[ch] then
				val = val + digitmap[ch] * factor
				factor = factor / base
			elseif ch == explower or ch == expupper then
				goto exponent
			elseif ch == '.' or letterordigit[ch] then
				goto fail
			else
				stream:ungetc(ch)
				goto success
			end
			ch = stream:getc()
		end
	end
::exponent::
	do
		local sign = 1
		local exp = 0
		ch = stream:getc()
		if ch == '-' then
			sign = -1
			ch = stream:getc()
		end
		if not decdigit[ch] then
			stream:ungetc(ch)
			goto fail
		end
		stream:ungetc(ch)
		while true do
			ch = stream:getc()
			if decdigit[ch] then
				exp = exp * 10 + decdigit[ch]
			elseif ch == '.' or letterordigit[ch] then
				goto fail
			else
				stream:ungetc(ch)
				val = val * math.pow(expbase, sign * exp)
				goto success
			end
		end
	end
::fail::
	stream:error('invalid hex number', spos, stream:getpos())
::success::
	return token:create('number', val, spos, stream:getpos())
end

local function obtaindecnumber(stream, spos)
	if not spos then
		spos = stream:getpos()
	end
	return obtainnumber_base(stream, 10, decdigit, 'e', 'E', 10, spos)
end

local function obtainhexnumber(stream, spos)
	return obtainnumber_base(stream, 16, hexdigit, 'p', 'P', 2, spos)
end

local function obtaincomment(stream, spos)
	local ch = stream:getc()
	if ch == '[' then
		local level = 0
		while true do
			ch = stream:getc()
			if ch == '[' then
				obtainlongstring(stream, level, spos)
				return
			elseif ch == '=' then
				level = level + 1
			else
				stream:ungetc(ch)
				break
			end
		end
	else
		stream:ungetc(ch)
	end
	repeat
		ch = stream:getc()
	until ch == '\n' or ch == ''
	stream:ungetc(ch)
end

local tokentable = {
	['\''] = obtainstring,
	['"'] = obtainstring,
}
for ch in pairs(letter) do
	tokentable[ch] = obtainword
end
for ch in pairs(decdigit) do
	tokentable[ch] = obtaindecnumber
end

tokentable['@'] = function(stream)
	local spos = stream:getpos()
	local ch = stream:getc()
	local word = {}
	while true do
		local ch = stream:getc()
		if letterordigit[ch] then
			table.append(word, ch)
		else
			stream:ungetc(ch)
			break
		end
	end
	word = table.concat(word)
	return token:create('identifier', word, spos, stream:getpos())
end

tokentable['['] = function(stream)
	local spos = stream:getpos()
	local open = stream:getc()
	local level = 0
	while true do
		local ch = stream:getc()
		if ch == '[' then
			return obtainlongstring(stream, level, spos)
		end
		if ch == '=' then
			level = level + 1
		else
			stream:ungetc(ch)
			for i = 1, level do
				stream:ungetc('=')
			end
			break
		end
	end
	return token:create('[', nil, spos, stream:getpos())
end

tokentable['0'] = function(stream)
	local spos = stream:getpos()
	local open = stream:getc()
	local ch = stream:getc()
	if ch == 'x' or ch == 'X' then
		return obtainhexnumber(stream, spos)
	else
		stream:ungetc(ch)
		stream:ungetc('0')
		return obtaindecnumber(stream, spos)
	end
end

tokentable['\n'] = function(stream)
	local spos = stream:getpos()
	stream:getc()
	return token:create('newline', nil, spos, stream:getpos())
end

tokentable['.'] = function(stream)
	local spos = stream:getpos()
	local open = stream:getc()
	local ch = stream:getc()
	if decdigit[ch] then
		stream:ungetc(ch)
		stream:ungetc(open)
		return obtaindecnumber(stream, spos)
	elseif ch ~= '.' then
		stream:ungetc(ch)
		return token:create('.', nil, spos, stream:getpos())
	end
	ch = stream:getc()
	if ch ~= '.' then
		stream:ungetc(ch)
		return token:create('..', nil, spos, stream:getpos())
	else
		return token:create('...', nil, spos, stream:getpos())
	end
end

tokentable['-'] = function(stream)
	local spos = stream:getpos()
	local open = stream:getc()
	local ch = stream:getc()
	if ch == '=' then
		return token:create('-=', nil, spos, stream:getpos())
	elseif ch == '-' then
		obtaincomment(stream, spos)
	else
		stream:ungetc(ch)
		return token:create('-', nil, spos, stream:getpos())
	end
end

local function obtainsymbol(stream)
	local spos = stream:getpos()
	local ch = stream:getc()
	return token:create(ch, nil, spos, stream:getpos())
end

local function obtaindigraph_gen(...)
	local set = table.makeset{...}
	return function(stream)
		local spos = stream:getpos()
		local open = stream:getc()
		local ch = stream:getc()
		local digraph = open .. ch
		if set[digraph] then
			return token:create(digraph, nil, spos, stream:getpos())
		else
			stream:ungetc(ch)
			return token:create(open, nil, spos, stream:getpos())
		end
	end
end

tokentable['('] = obtainsymbol
tokentable[')'] = obtainsymbol
tokentable[']'] = obtainsymbol
tokentable['{'] = obtainsymbol
tokentable['}'] = obtainsymbol
tokentable[';'] = obtainsymbol
tokentable[':'] = obtainsymbol
tokentable['!'] = obtainsymbol
tokentable['='] = obtaindigraph_gen('==')
tokentable['+'] = obtaindigraph_gen('+=')
tokentable['*'] = obtaindigraph_gen('*=')
tokentable['/'] = obtaindigraph_gen('/=')
tokentable['~'] = obtaindigraph_gen('~=')
tokentable['#'] = obtainsymbol
tokentable[','] = obtainsymbol
tokentable[''] = function(stream)
	local pos = stream:getpos()
	return token:create('eof', nil, pos, pos)
end

function lexer.obtaintoken(stream)
	local bislinestart = false
	while true do
		local ch = stream:getc()
		local tfunc = tokentable[ch]
		if tfunc then
			stream:ungetc(ch)
			local token = tfunc(stream)
			if token and token:gettype() == 'newline' then
				bislinestart = true
			elseif token then
				token.bislinestart = bislinestart
				return token
			end
		elseif not whitespace[ch] then
			stream:ungetc(ch)
			stream:error('invalid symbol', stream:getpos())
		end
	end
end
