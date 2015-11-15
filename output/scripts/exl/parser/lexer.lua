local modname = ...
local lexer = package.modtable(modname)
local object = require('base.object')
local token = require('exl.parser.token')
local utf8 = require('utf8')

-- input stream interface:
-- function getc() - returns nil/false on eof
-- function ungetc()
-- function getpos() - returns the current position
-- context interface:
-- function log(level, msg, pos) - prints a message with a reference to the specified position
-- possible level values are 'error' and 'warning'

--[[
exl tokens:
	'identifier'
	'number' - contains the value
	'string' - contains the converted payload
	'eof'
	keywords - each one is a distinct type
	symbols and digraphs - each one is a distinct type

	'newline' - internal token, is not returned by lexer.obtaintoken
	instead, token:islinestart of the next one returns true

comments and blanks are ignored
]]

local keyword = {
	['const'] = true,
	['end'] = true,
	['function'] = true,
	['local'] = true,
	['nil'] = true,
	['unit'] = true,
	['void'] = true,
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

local function obtaincharacter(stream, context, char)
	local c = stream:getc()
	if c == char then
		return true
	else
		stream:ungetc(c)
		context:log('error', char..' expected', stream:getpos())
		return false
	end
end

local function obtaindec(stream, context, length)
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

local function obtainhexfixed(stream, context, length)
	local ret = 0
	for i = 1, length do
		local ch = stream:getc()
		if hexdigit[ch] then
			ret = ret * 16 + hexdigit[ch]
		else
			stream:ungetc(ch)
			context:log('error', 'invalid hex digit', stream:getpos())
			return nil
		end
	end
	return ret
end

local function obtainhex(stream, context, length)
	local ret = 0
	for i = 1, length do
		local ch = stream:getc()
		if hexdigit[ch] then
			ret = ret * 16 + hexdigit[ch]
		else
			stream:ungetc(ch)
			if i == 1 then
				context:log('error', 'invalid hex digit', stream:getpos())
				return nil
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
local function obtainstringelement(stream, context, first)
	if first == '\\' then
		local ch = stream:getc()
		local value = stringelement_escape[ch]
		if value then
			return value
		elseif ch == 'x' then
			local val = obtainhexfixed(stream, context, 2)
			return val and string.char(val) or ''
		elseif decdigit[ch] then
			stream:ungetc(ch)
			local val = obtaindec(stream, context, 1, 3)
			if val > 255 then
				context:log('error', 'invalid byte value', stream:getpos())
				return ''
			else
				return string.char(val)
			end
		elseif ch == 'u' then
			if not obtaincharacter(stream, context, '{') then
				return ''
			end
			local val = obtainhex(stream, context, 8)
			if not obtaincharacter(stream, context, '}') then
				return ''
			end
			local r = utf8.encodechar(val)
			if r then
				return r
			else
				context:log('error', 'invalid code point', stream:getpos())
				return ''
			end
		elseif ch == 'z' then
			repeat
				ch = stream:getc()
			until not whitespace[ch]
			stream:ungetc(ch)
			return ''
		else
			context:log('error', 'unknown escape character', stream:getpos())
			return ''
		end
	else
		return first
	end
end

local function obtainstring(stream, context)
	local spos = stream:getpos()
	local open = stream:getc()
	local content = {}
	while true do
		local ch = stream:getc()
		if ch == open then
			break
		elseif ch == '\n' or ch == '' then
			context:log('error', 'unterminated string', stream:getpos())
			stream:ungetc(ch)
			break
		end
		table.append(content, obtainstringelement(stream, context, ch))
	end
	return token:create('string', table.concat(content), spos, stream:getpos())
end

local function obtainlongstring(stream, context, level, spos)
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
			context:log('error', 'unterminated long string', stream:getpos())
			break
		else
			table.append(content, ch)
		end
	end
	return token:create('string', table.concat(content), spos, stream:getpos())
end

local function obtainword(stream, context)
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
	if keyword[word] then
		return token:create(word, nil, spos, stream:getpos())
	else
		return token:create('identifier', word, spos, stream:getpos())
	end
end

local function skipinvalidnumber(stream, context)
	local ch
	repeat
		ch = stream:getc()
	until not letterordigit[ch] and ch ~= '.'
	stream:ungetc(ch)
end

local function obtainnumber_base(
		stream, context, base, digitmap, explower, expupper, expbase, spos)
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
	do
		context:log('error', 'invalid hex number', stream:getpos())
		skipinvalidnumber(stream, context)
		return token:create('number', 0, spos, stream:getpos())
	end
::success::
	return token:create('number', val, spos, stream:getpos())
end

local function obtaindecnumber(stream, context, spos)
	if not spos then
		spos = stream:getpos()
	end
	return obtainnumber_base(stream, context, 10, decdigit, 'e', 'E', 10, spos)
end

local function obtainhexnumber(stream, context, spos)
	return obtainnumber_base(stream, context, 16, hexdigit, 'p', 'P', 2, spos)
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

tokentable['['] = function(stream, context)
	local spos = stream:getpos()
	local open = stream:getc()
	local level = 0
	while true do
		local ch = stream:getc()
		if ch == '[' then
			return obtainlongstring(stream, context, level, spos)
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

tokentable['0'] = function(stream, context)
	local spos = stream:getpos()
	local open = stream:getc()
	local ch = stream:getc()
	if ch == 'x' or ch == 'X' then
		return obtainhexnumber(stream, context, spos)
	else
		stream:ungetc(ch)
		stream:ungetc('0')
		return obtainnumber(stream, context, spos)
	end
end

tokentable['.'] = function(stream, context)
	local spos = stream:getpos()
	local open = stream:getc()
	local ch = stream:getc()
	stream:ungetc(ch)
	if decdigit[ch] then
		stream:ungetc(open)
		return obtaindecnumber(stream, context, spos)
	else
		return token:create(open, nil, spos, stream:getpos())
	end
end

tokentable['\n'] = function(stream, context)
	local spos = stream:getpos()
	stream:getc()
	return token:create('newline', nil, spos, stream:getpos())
end

tokentable['.'] = function(stream, context)
	local spos = stream:getpos()
	local open = stream:getc()
	local ch = stream:getc()
	if ch ~= '.' then
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

local function obtainsymbol(stream, context)
	local spos = stream:getpos()
	local ch = stream:getc()
	return token:create(ch, nil, spos, stream:getpos())
end

local function obtaindigraph_gen(...)
	local set = table.makeset{...}
	return function(stream, context)
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
tokentable['='] = obtaindigraph_gen('==')
tokentable['+'] = obtaindigraph_gen('+=')
tokentable['-'] = obtaindigraph_gen('-=')
tokentable['~'] = obtaindigraph_gen('~=')
tokentable['#'] = obtainsymbol
tokentable[''] = function(stream, context)
	local pos = stream:getpos()
	return token:create('eof', nil, pos, pos)
end

function lexer.obtaintoken(stream, context)
	local bislinestart = false
	while true do
		local ch = stream:getc()
		local tfunc = tokentable[ch]
		if tfunc then
			stream:ungetc(ch)
			local token = tfunc(stream, context)
			if token:gettype() == 'newline' then
				bislinestart = true
			elseif token then
				token.bislinestart = bislinestart
				return token
			end
		elseif not whitespace[ch] then
			context:log('error', 'invalid symbol', stream:getpos())
		end
	end
end
