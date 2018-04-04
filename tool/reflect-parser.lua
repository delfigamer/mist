local modname = ...
local parserlib
if modname then
	parserlib = package.modtable(modname)
else
	assert(loadfile('tool/base.lua'))()
	setfenv(1, _G)
	protectglobaltable(true)
	parserlib = package.modtable('reflect-parser')
end
local env = require('env')



local function stage_mainwrap(main, ...)
	coroutine.yield()
	local suc, err = pcall(main, ...)
	if not suc then
		error(err)
	end
	while true do
		coroutine.yield()
	end
end

local function stage(main, ...)
	local thread = coroutine.wrap(stage_mainwrap)
	thread(main, ...)
	return function()
		local suc, ret = pcall(thread)
		if suc then
			return ret
		else
			ret.traceback = ret.traceback .. debug.traceback('')
			error(ret)
		end
	end
end

local function copy(list)
	local new = {}
	for i, v in ipairs(list) do
		new[i] = v
	end
	return new
end

local function locationstring(loc)
	local lines = {}
	for i, inst in ipairs(loc) do
		table.append(lines, string.format('\n\t%s:%i:%i:',
			inst[1], inst[2], inst[3]))
	end
	return table.concat(lines)
end

local function locationerror(loc, str)
	error(locationstring(loc)..'\n'..str)
end

local function tokenstring(token)
	local locstr = locationstring(token.location)
	if token.word then
		return locstr..'\n'..token.word
	elseif token.numberliteral then
		return locstr..'\n'..token.numberliteral
	elseif token.stringliteral then
		local qstr = string.format('%q', token.stringliteral)
		return locstr..'\n'..qstr
	elseif token.charliteral then
		local qstr = string.format('%q', token.charliteral)
		return locstr..'\n\''..string.sub(qstr, 2, -2)..'\''
	elseif token.punctuator then
		return locstr..'\n'..token.punctuator
	elseif token.directive then
		return locstr..'\n#'..token.directive
	elseif token.include then
		return locstr..'\n#include <'..token.include..'>'
	elseif token.remit then
		return locstr..'\n'..token.remit..'(<<'..token.payload..'>>)'
	elseif token.whitespace then
		return locstr
	elseif token.newline then
		return locstr..'\n\n'
	else
		return locstr..'\n< unknown token >'
	end
end

local function namestring(name)
	if not name then
		return '<>'
	end
	local str = table.concat(name, '::')
	if name.isfull then
		str = '::' .. str
	end
	return str
end

local function scopednamestring(scope, name)
	if name.isfull then
		return namestring(name)
	end
	local str = ''
	for i, part in ipairs(scope) do
		str = str .. '::' .. part
	end
	for i, part in ipairs(name) do
		str = str .. '::' .. part
	end
	return str
end



local function reader_main(path)
	local file = assert(io.open(env.path(path), 'r'))
	while true do
		local ch = file:read(1)
		if not ch then
			break
		end
		coroutine.yield(ch)
	end
	coroutine.yield('\n')
end

local function reader(path)
	return stage(reader_main, path)
end



local function iswhitespace(ch)
	local n = string.byte(ch)
	return n <= 32 and n ~= 10
end

local function isletter(ch)
	local n = string.byte(ch)
	return
		n >= string.byte('A') and n <= string.byte('Z') or
		n >= string.byte('a') and n <= string.byte('z') or
		n == string.byte('_')
end

local function isdigit(ch)
	local n = string.byte(ch)
	return
		n >= string.byte('0') and n <= string.byte('9')
end

local function isalphanumeric(ch)
	return isletter(ch) or isdigit(ch)
end

local keywordset = {}
for kw in string.gmatch([[
	alignas      continue      friend     register          true
	alignof      decltype      goto       reinterpret_cast  try
	asm          default       if         return            typedef
	auto         delete        inline     short             typeid
	bool         do            int        signed            typename
	break        double        long       sizeof            union
	case         dynamic_cast  mutable    static            unsigned
	catch        else          namespace  static_assert     using
	char         enum          new        static_cast       virtual
	char16_t     explicit      noexcept   struct            void
	char32_t     export        nullptr    switch            volatile
	class        extern        operator   template          wchar_t
	const        false         private    this              while
	constexpr    float         protected  thread_local
	const_cast   for           public     throw]], '%S+')
do
	keywordset[kw] = true
end

local alternativewordset = {
	['and'] = '&&',
	['and_eq'] = '&=',
	['bitand'] = '&',
	['bitor'] = '|',
	['compl'] = '~',
	['not'] = '!',
	['not_eq'] = '!=',
	['or'] = '||',
	['or_eq'] = '|=',
	['xor'] = '^',
	['xor_eq'] = '^='}

local punctuatorprefixset = {}
local punctuatorset = {}
for kw in string.gmatch([[
	{    }    [    ]    #    ##     (     )
	<:   :>   <%   %>   %:   %:%:   ;     :     ...
	?    ::   .    .*
	+    -    *    /    %    ˆ      &     |     ~
	!    =    <    >    +=   -=     *=    /=    %=
	ˆ=   &=   |=   <<   >>   >>=    <<=   ==    !=
	<=   >=   &&   ||   ++   --     ,     ->*   ->]], '%S+')
do
	for i = 1, #kw do
		punctuatorprefixset[string.sub(kw, 1, i)] = true
	end
	punctuatorset[kw] = true
end

local alternativepunctuatorset = {
	['<%'] = '{',
	['%>'] = '}',
	['<:'] = '[',
	[':>'] = ']',
	['%:'] = '#',
	['%:%:'] = '##'}

local emittargetset = {
	['r_emit'] = true}
-- for kw in string.gmatch([[
	-- hpp_start           hpp_includes        hpp_classes         hpp_mldecl
	-- hpp_mldeclstart     hpp_mldeclend       hpp_mldef           hpp_end
	-- cpp_start           cpp_methods         cpp_mldef           cpp_end
	-- ffi_start           ffi_mldecl          ffi_mldeclstart     ffi_mldeclend
	-- ffi_end             lua_start           lua_classes         lua_methods
	-- lua_metatypes       lua_end]], '%S+')
-- do
	-- emittargetset['r_emit_'..kw] = true
-- end

local function lexer_main(path)
	local source = reader(path)
	local ch = '\n'
	local nc = source()
	local crow = 0
	local ccol = 1

	local function advance()
		if ch == '\n' then
			crow = crow + 1
			ccol = 1
		else
			ccol = ccol + 1
		end
		ch = nc
		nc = source()
		if ch == '\\' and nc == '\n' then
			crow = crow + 1
			ccol = 1
			ch = source()
			nc = source()
		end
	end

	local function location()
		return {{path, crow, ccol}}
	end

	local function location_back(offset)
		return {{path, crow, ccol - offset}}
	end

	local function lerror(str)
		locationerror(location(), str)
	end

	local function escapechar()
		if ch == 'a' then
			advance()
			return '\x07'
		elseif ch == 'b' then
			advance()
			return '\x08'
		elseif ch == 'f' then
			advance()
			return '\x0a'
		elseif ch == 'n' then
			advance()
			return '\x0a'
		elseif ch == 'r' then
			advance()
			return '\x0d'
		elseif ch == 't' then
			advance()
			return '\x09'
		elseif ch == 'v' then
			advance()
			return '\x0b'
		else
			advance()
			return ch
		end
	end

	local function readstring(tagname, termchar)
		local payload = ''
		local start = location()
		advance()
		while ch ~= termchar do
			if ch == '\\' then
				advance()
				payload = payload .. escapechar()
			elseif ch == '\n' then
				lerror('unterminated string literal')
			else
				payload = payload .. ch
				advance()
			end
		end
		advance()
		coroutine.yield{[tagname]=payload, location = start}
	end

	local function readnumber()
		local payload = ''
		local start = location()
		while true do
			if ch == 'e' or ch == 'E' then
				payload = payload .. ch
				advance()
				if isalphanumeric(ch) or ch == '+' or ch == '-' then
					payload = payload .. ch
				else
					break
				end
			elseif isalphanumeric(ch) or ch == '.' then
				payload = payload .. ch
			else
				break
			end
			advance()
		end
		coroutine.yield{numberliteral=payload, location=start}
	end

	local function readword()
		local payload = ''
		local start = location()
		if ch == '~' then
			advance()
			if not isletter(ch) then
				coroutine.yield{punctuator='~', location=start}
				return
			end
			payload = '~'
		end
		while isalphanumeric(ch) do
			payload = payload .. ch
			advance()
		end
		if alternativewordset[payload] then
			coroutine.yield{punctuator=alternativewordset[payload], location=start}
		elseif emittargetset[payload] then
			local ttype = payload
			local fallbacktokenlist = {{word=payload, location=start}}
			if iswhitespace(ch) then
				table.append(fallbacktokenlist,
					{whitespace=true, location=location()})
				while iswhitespace(ch) and ch ~= '\n' do
					advance()
				end
			end
			if ch ~= '(' then
				goto prefixmismatch
			end
			table.append(fallbacktokenlist,
				{punctuator='(', location=location()})
			advance()
			if ch ~= '<' then
				goto prefixmismatch
			end
			table.append(fallbacktokenlist,
				{punctuator='<', location=location()})
			advance()
			if ch ~= '<' then
				goto prefixmismatch
			end
			advance()
			payload = ''
			while true do
				if ch == '>' then
					advance()
					if ch == '>' then
						advance()
						if ch == ')' then
							advance()
							break
						elseif not ch then
							lerror('unterminated r_emit')
						else
							payload = payload .. '>>' .. ch
						end
					elseif not ch then
						lerror('unterminated r_emit')
					else
						payload = payload .. '>' .. ch
					end
				elseif not ch then
					lerror('unterminated r_emit')
				else
					payload = payload .. ch
				end
				advance()
			end
			coroutine.yield{remit=ttype, payload=payload, location=start}
			do return end

		::prefixmismatch::
			for i, token in ipairs(fallbacktokenlist) do
				coroutine.yield(token)
			end
			return
		else
			coroutine.yield{word=payload, location=start}
		end
	end

	local function readwhitespace()
		local start = location()
		while iswhitespace(ch) do
			advance()
		end
		coroutine.yield{whitespace=true, location=start}
	end

	local function readlinecomment()
		local start = location_back(2)
		while ch ~= '\n' do
			advance()
		end
		coroutine.yield{whitespace=true, location=start}
	end

	local function readblockcomment()
		local start = location_back(2)
		while true do
			if ch == '*' then
				advance()
				if ch == '/' then
					break
				elseif not ch then
					lerror('unterminated multi-line comment')
				end
			elseif not ch then
				lerror('unterminated multi-line comment')
			end
			advance()
		end
		advance()
		coroutine.yield{whitespace=true, location=start}
	end

	local function readdirective()
		local payload = ''
		local start = location_back(1)
		while isalphanumeric(ch) do
			payload = payload .. ch
			advance()
		end
		if payload ~= 'include' then
			coroutine.yield{directive=payload, location=start}
			return
		end
		while iswhitespace(ch) do
			advance()
		end
		local termchar
		if ch == '"' then
			lerror('#include "" are not implemented, use <> instead')
			-- termchar = '"'
			-- payload = 'local:'
		elseif ch == '<' then
			termchar = '>'
			payload = ''
		else
			lerror('invalid #include directive')
		end
		advance()
		while ch ~= termchar do
			if ch == '\n' then
				lerror('unterminated #include path')
			end
			payload = payload .. ch
			advance()
		end
		advance()
		coroutine.yield{include=payload, location=start}
	end

	local function readpunctuator()
		local current = ''
		local start = location()
		while true do
			local next = current .. ch
			if punctuatorprefixset[next] then
				current = next
				advance()
			else
				break
			end
		end
		if current == '' then
			lerror('invalid symbol')
		elseif alternativepunctuatorset[payload] then
			coroutine.yield{punctuator=alternativepunctuatorset[payload],
				location=start}
		else
			coroutine.yield{punctuator=current, location=start}
		end
	end

	while true do
		if ch == '\n' then
			coroutine.yield{newline=true, location=location()}
			advance()
			while ch and iswhitespace(ch) do
				advance()
			end
			if ch == '#' then
				advance()
				readdirective()
			elseif not ch then
				break
			end
		elseif ch == '/' then
			advance()
			if ch == '/' then
				advance()
				readlinecomment()
			elseif ch == '*' then
				advance()
				readblockcomment()
			elseif ch == '=' then
				coroutine.yield{punctuator='/=', location=location_back(1)}
				advance()
			else
				coroutine.yield{punctuator='/', location=location()}
			end
		elseif ch == '\'' then
			readstring('charliteral', '\'')
		elseif ch == '"' then
			readstring('stringliteral', '"')
		elseif iswhitespace(ch) then
			readwhitespace()
		elseif isdigit(ch) then
			readnumber()
		elseif isletter(ch) or ch == '~' then
			readword()
		else
			readpunctuator()
		end
	end
end

local function lexer(path)
	return stage(lexer_main, path)
end



local function preprocessor_main(path)
	local source = lexer(path)
	local token = source()
	local attrdepth

	local function advance()
		token = source()
	end

	local function lerror(str)
		locationerror(token.location, str)
	end

	while token do
		if token.whitespace or token.newline then
			advance()
		elseif token.directive then
			while not token.newline do
				advance()
			end
		else
			if attrdepth then
				if token.punctuator == '[' then
					attrdepth = attrdepth + 1
					coroutine.yield(token)
					advance()
				else
					if token.punctuator == ']' then
						attrdepth = attrdepth - 1
						if attrdepth == 0 then
							local starttoken = token
							advance()
							if token.punctuator == ']' then
								starttoken.punctuator = ']]'
								attrdepth = nil
								coroutine.yield(starttoken)
								advance()
							else
								lerror('attribute sequence is not properly terminated')
							end
						else
							coroutine.yield(token)
							advance()
						end
					else
						coroutine.yield(token)
						advance()
					end
				end
			else
				if token.punctuator == '[' then
					local starttoken = token
					advance()
					if token.punctuator == '[' then
						starttoken.punctuator = '[['
						attrdepth = 1
						coroutine.yield(starttoken)
						advance()
					else
						coroutine.yield(starttoken)
					end
				else
					if token.word and not keywordset[token.word] then
						token.ident = token.word
					end
					coroutine.yield(token)
					advance()
				end
			end
		end
	end
end

local function preprocessor(path)
	return stage(preprocessor_main, path)
end



local function decltypestring(decltype, name)
	local prefix = ''
	local suffix = ''
	local center = name or ''
	local needsbrackets = false

	local function addprefix(part)
		needsbrackets = true
		prefix = part .. prefix
	end

	local function addsuffix(part)
		if needsbrackets then
			needsbrackets = false
			center = '(' .. prefix .. ' ' .. center .. suffix .. ')'
			prefix = ''
			suffix = part
		else
			suffix = part .. suffix
		end
	end

	local function cvstr()
		local part = ''
		if decltype.isconst then
			part = part .. ' const'
		end
		if decltype.isvolatile then
			part = part .. ' volatile'
		end
		return part
	end

	local function combine(basestr)
		return basestr .. prefix .. ' ' .. center .. suffix
	end

	while true do
		if decltype.fundamental then
			return combine(decltype.fundamental .. cvstr())
		elseif decltype.name then
			return combine(namestring(decltype.name) .. cvstr())
		elseif decltype.template then
			local argstr = {}
			for i, arg in ipairs(decltype.args) do
				argstr[i] = decltypestring(arg)
			end
			return combine(
				namestring(decltype.template) ..
				'<' .. table.concat(argstr, ', ') .. '>' ..
				cvstr())
		elseif decltype.pointer then
			addprefix('*' .. cvstr())
			decltype = decltype.pointer
		elseif decltype.lref then
			addprefix('&')
			decltype = decltype.lref
		elseif decltype.rref then
			addprefix('&&')
			decltype = decltype.rref
		elseif decltype.array then
			if decltype.size then
				addsuffix('[' .. tostring(decltype.size) .. ']')
			else
				addsuffix('[]')
			end
			decltype = decltype.array
		elseif decltype.functionret then
			local argstr = {}
			for i, arg in ipairs(decltype.args) do
				if arg.name then
					argstr[i] = decltypestring(arg.type, namestring(arg.name))
				else
					argstr[i] = decltypestring(arg.type)
				end
			end
			local mod = cvstr()
			if decltype.isnoexcept then
				mod = mod .. ' noexcept'
			end
			addsuffix('(' .. table.concat(argstr, ', ') .. ')' .. mod)
			if decltype.convention then
				addprefix(' ' .. decltype.convention)
			end
			decltype = decltype.functionret
		else
			return combine('/*unknown*/ void')
		end
	end
end

local fundamentaltypeset = {
	['char'] = 'char',
	['unsigned char'] = 'unsigned char',
	['signed char'] = 'signed char',
	['char16_t'] = 'char16_t',
	['char32_t'] = 'char32_t',
	['bool'] = 'bool',
	['unsigned'] = 'unsigned int',
	['unsigned int'] = 'unsigned int',
	['signed'] = 'int',
	['signed int'] = 'int',
	['int'] = 'int',
	['unsigned short int'] = 'unsigned short int',
	['unsigned short'] = 'unsigned short int',
	['unsigned long int'] = 'unsigned long int',
	['unsigned long'] = 'unsigned long int',
	['unsigned long long int'] = 'unsigned long long int',
	['unsigned long long'] = 'unsigned long long int',
	['signed long int'] = 'long int',
	['signed long'] = 'long int',
	['signed long long int'] = 'long long int',
	['signed long long'] = 'long long int',
	['long long int'] = 'long long int',
	['long long'] = 'long long int',
	['long int'] = 'long int',
	['long'] = 'long int',
	['signed short int'] = 'short int',
	['signed short'] = 'short int',
	['short int'] = 'short int',
	['short'] = 'short int',
	['wchar_t'] = 'wchar_t',
	['float'] = 'float',
	['double'] = 'double',
	['long double'] = 'long double',
	['void'] = 'void',
}

local delcspecifierset = {
	['typedef'] = true,
	['extern'] = true,
	['static'] = true,
	['friend'] = true,
	['mutable'] = true,
	['inline'] = true,
	['explicit'] = true,
	['virtual'] = true,
}

local function parser_main(path)
	local source = preprocessor(path)
	local token = source()

	local function advance()
		token = source()
	end

	local function lerror(str)
		locationerror(token.location, str)
	end

	local function readnumber()
		local str = token.numberliteral
		if not str then
			lerror('number expected')
		end
		local value = tonumber(str)
		if not value then
			lerror('invalid number token')
		end
		advance()
		return value
	end

	local function skipexpression()
		local depth = 0
		while true do
			if depth == 0 then
				if token.punctuator == ',' or
					token.punctuator == ';' or
					token.punctuator == ')'
				then
					break
				elseif token.punctuator == '(' then
					depth = depth + 1
					advance()
				else
					advance()
				end
			else
				if token.punctuator == ')' then
					depth = depth - 1
					advance()
				elseif token.punctuator == '(' then
					depth = depth + 1
					advance()
				else
					advance()
				end
			end
		end
	end

	local function readname()
		local name = {}
		if token.punctuator == '::' then
			name.isfull = true
			advance()
		elseif not token.word then
			return
		end
		while true do
			if token.word == 'operator' then
				advance()
				if token.word then
					table.append(name, 'operator ' .. token.word)
				elseif token.punctuator then
					table.append(name, 'operator' .. token.punctuator)
				elseif token.stringliteral then
					if token.stringliteral ~= '' then
						lerror('operator name expected')
					end
					advance()
					if not token.word then
						lerror('operator name expected')
					end
					table.append(name, 'operator"" ' .. token.word)
				else
					lerror('operator name expected')
				end
				advance()
				break
			elseif token.word then
				table.append(name, token.word)
				advance()
				if token.punctuator ~= '::' then
					break
				end
				advance()
			else
				lerror('name expected')
			end
		end
		return name
	end

	local function readlistcontent(elementfunc, terminator, ...)
		if token.punctuator == terminator then
			return
		end
		while true do
			elementfunc(...)
			if token.punctuator == terminator then
				return
			elseif token.punctuator == ',' then
				advance()
			else
				lerror(', expected')
			end
		end
	end

	local function readlist(...)
		advance()
		readlistcontent(...)
		advance()
	end

	local function readattributeparam(params)
		if token.stringliteral then
			table.append(params, token.stringliteral)
			advance()
		else
			lerror('unsupported attribute parameter')
		end
	end

	local function readattribute(attrs)
		local name = {}
		while true do
			if not token.word then
				lerror('attribute name expected')
			end
			table.append(name, token.word)
			advance()
			if token.punctuator == '::' then
				advance()
			else
				break
			end
		end
		name = table.concat(name, '::')
		local params = false
		if token.punctuator == '(' then
			params = {}
			readlist(readattributeparam, ')', params)
		end
		attrs[name] = params
	end

	local function readattrs(attrs)
		if not attrs then
			attrs = {}
		end
		while token.punctuator == '[[' do
			readlist(readattribute, ']]', attrs)
		end
		return attrs
	end

	local --[[function]] readtemplateargs --()

	local function readcvqualifier(ent)
		while true do
			if token.word == 'const' then
				if ent.isconst then
					lerror('multiple const qualifiers')
				else
					ent.isconst = true
				end
				advance()
			elseif token.word == 'volatile' then
				if ent.isvolatile then
					lerror('multiple volatile qualifiers')
				else
					ent.isvolatile = true
				end
				advance()
			else
				break
			end
		end
	end

	local function readbasetype()
		local result = {}
		local name
		readcvqualifier(result)
		if fundamentaltypeset[token.word] then
			local current = token.word
			advance()
			while true do
				if token.word then
					local next = current .. ' ' .. token.word
					if fundamentaltypeset[next] then
						current = next
						advance()
					else
						break
					end
				else
					break
				end
			end
			result.fundamental = fundamentaltypeset[current]
			readcvqualifier(result)
			return result
		elseif token.ident or token.punctuator == '::' then
			local name = readname()
			if token.punctuator == '<' then
				result.template = name
				result.args = readtemplateargs()
			else
				result.name = name
			end
			readcvqualifier(result)
		else
			lerror('base type expected')
		end
		return result
	end

	local --[[function]] readargdecl --(args)

	local function readfunctionsuffix(subdecl)
		local modifier = {
			mod = 'function'}
		modifier.args = {}
		readlistcontent(readargdecl, ')', modifier.args)
		advance()
		readcvqualifier(modifier)
		if token.word == 'noexcept' then
			modifier.isnoexcept = true
			advance()
		end
		table.append(subdecl.modifiers, modifier)
	end

	local function readsubdecl(attrs)
		local subdecl = {
			modifiers = {}}
		while true do
			if token.punctuator == '*' then
				local modifier = {
					mod = 'pointer'}
				advance()
				readcvqualifier(modifier)
				table.append(subdecl.modifiers, modifier)
			elseif token.punctuator == '&' then
				local modifier = {
					mod = 'lref'}
				advance()
				table.append(subdecl.modifiers, modifier)
			elseif token.punctuator == '&&' then
				local modifier = {
					mod = 'rref'}
				advance()
				table.append(subdecl.modifiers, modifier)
			elseif token.word == 'CALLBACK' then
				local modifier = {
					mod = 'convention',
					name = token.word}
				advance()
				table.append(subdecl.modifiers, modifier)
			elseif token.punctuator == '(' then
				advance()
				if
					token.punctuator == '*' or
					token.punctuator == '&' or
					token.punctuator == '&&' or
					token.punctuator == '('
				then
					subdecl.subdecl = readsubdecl()
					if token.punctuator ~= ')' then
						lerror(') expected')
					end
					advance()
					break
				else
					readfunctionsuffix(subdecl)
					break
				end
			elseif
				token.word == 'operator' or
				token.ident or token.punctuator == '::'
			then
				subdecl.name = readname()
				readattrs(attrs)
				break
			else
				break
			end
		end
		while true do
			if token.punctuator == '[' then
				local modifier = {
					mod = 'array'}
				advance()
				if token.punctuator ~= ']' then
					modifier.size = readnumber()
					if token.punctuator ~= ']' then
						lerror('] expected')
					end
				end
				advance()
				table.append(subdecl.modifiers, modifier)
			elseif token.punctuator == '(' then
				advance()
				readfunctionsuffix(subdecl)
				break
			else
				break
			end
		end
		return subdecl
	end

	local function readdeclarator(basetype, attrs)
		local subdecl = readsubdecl(attrs)
		local decltype = basetype
		local name
		repeat
			for i, modifier in ipairs(subdecl.modifiers) do
				if modifier.mod == 'pointer' then
					decltype = {
						pointer = decltype,
						isconst = modifier.isconst,
						isvolatile = modifier.isvolatile}
				elseif modifier.mod == 'lref' then
					decltype = {
						lref = decltype}
				elseif modifier.mod == 'rref' then
					decltype = {
						rref = decltype}
				elseif modifier.mod == 'convention' then
					decltype.convention = modifier.name
				elseif modifier.mod == 'array' then
					decltype = {
						array = decltype,
						size = modifier.size}
				elseif modifier.mod == 'function' then
					decltype = {
						functionret = decltype,
						args = modifier.args,
						convention = decltype.convention,
						isconst = modifier.isconst,
						isvolatile = modifier.isvolatile,
						isnoexcept = modifier.isnoexcept}
				else
					lerror('unknown modifier '..modifier.mod)
				end
			end
			if subdecl.name then
				name = subdecl.name
			end
			subdecl = subdecl.subdecl
		until not subdecl
		return {
			name = name,
			type = decltype}
	end

	--[[local]] function readargdecl(args)
		local location = token.location
		local attrs = readattrs()
		local basetype = readbasetype()
		local declarator = readdeclarator(basetype, attrs)
		declarator.attrs = attrs
		declarator.location = location
		table.append(args, declarator)
		if token.punctuator == '=' then
			advance()
			skipexpression()
		end
	end

	--[[local]] function readtemplateargs()
		advance()
		if token.punctuator == '>' then
			advance()
			return {}
		elseif token.punctuator == '>>' then
			token.punctuator = '>'
			return {}
		end
		local args = {}
		while true do
			local basetype = readbasetype()
			local declarator = readdeclarator(basetype)
			table.append(args, declarator.type)
			if token.punctuator == ',' then
				advance()
			elseif token.punctuator == '>' then
				advance()
				return args
			elseif token.punctuator == '>>' then
				token.punctuator = '>'
				return args
			else
				lerror('> expected')
			end
		end
	end

	local function readdeclspecifiers()
		local specs = {}
		while delcspecifierset[token.word] do
			if specs[token.word] then
				lerror('multiple '..token.word..' specifiers')
			else
				specs[token.word] = true
			end
			advance()
		end
		return specs
	end

	local function readdeclaration(scope, attrs, specs)
		local location = token.location
		if specs.typedef and
			(specs.extern or specs.static or specs.mutable or specs.inline)
		then
			lerror('invalid specifier keywords')
		end
		local basetype = readbasetype()
		if token.punctuator == ';' then
			advance()
			return
		end
		while true do
			local declarator = readdeclarator(basetype)
			declarator.location = location
			declarator.attrs = attrs
			if specs.typedef then
				declarator.typedef = true
			else
				declarator.object = true
				declarator.isstatic = specs.static
				declarator.isfriend = specs.friend
				declarator.isvirtual = specs.virtual
			end
			if not declarator.name then
				declarator.incomplete = true
			end
			declarator.scope = scope
			coroutine.yield(declarator)
			if token.word == 'override' then
				advance()
			end
			if token.punctuator == '=' then
				advance()
				skipexpression()
			end
			if token.punctuator == ';' then
				advance()
				return
			elseif token.punctuator == '{' or token.punctuator == ':' then
				local depth = 0
				while true do
					if token.punctuator == '(' then
						depth = depth + 1
						advance()
					elseif token.punctuator == ')' then
						depth = depth - 1
						advance()
					elseif token.punctuator == '{' then
						if depth == 0 then
							break
						end
						advance()
					else
						advance()
					end
				end
				while true do
					if token.punctuator == '{' then
						depth = depth + 1
						advance()
					elseif token.punctuator == '}' then
						depth = depth - 1
						advance()
						if depth == 0 then
							return
						end
					else
						advance()
					end
				end
			elseif token.punctuator ~= ',' then
				lerror('; expected')
			end
			advance()
		end
	end

	local --[[function]] readdeclregion --(scope)
	local --[[function]] readenumregion --(scope)

	local function readclass(scope)
		local classdecl = {
			class = true,
			location = token.location,
			scope = scope,
			bases = {}}
		advance()
		classdecl.attrs = readattrs()
		if token.word or token.punctuator == '::' then
			classdecl.name = readname()
		end
		if token.punctuator == ';' then
			advance()
			coroutine.yield(classdecl)
			return
		end
		if token.punctuator == ':' then
			advance()
			while true do
				if token.word == 'private' or
					token.word == 'protected' or
					token.word == 'public'
				then
					advance()
				end
				local name = readname()
				if not name then
					lerror('base class name expected')
				end
				table.append(classdecl.bases, name)
				if token.punctuator == ',' then
					advance()
				else
					break
				end
			end
		end
		coroutine.yield(classdecl)
		local subscope = copy(scope)
		subscope.class = classdecl
		if not classdecl.name then
			lerror('class name is required')
		end
		for i, part in ipairs(classdecl.name) do
			table.append(subscope, part)
		end
		if token.punctuator ~= '{' then
			lerror('{ expected')
		end
		advance()
		readdeclregion(subscope)
		if token.punctuator ~= '}' then
			lerror('} expected')
		end
		advance()
		if token.punctuator ~= ';' then
			lerror('; expected')
		end
		advance()
	end

	local function readenum(scope)
		local enumdecl = {
			location = token.location,
			scope = scope}
		advance()
		if token.word == 'class' or token.word == 'struct' then
			enumdecl.enumclass = true
			advance()
		else
			enumdecl.enumtype = true
		end
		enumdecl.attrs = readattrs()
		if token.word or token.punctuator == '::' then
			enumdecl.name = readname()
		end
		if token.punctuator == ';' then
			advance()
			coroutine.yield(enumdecl)
			return
		end
		if token.punctuator == ':' then
			lerror('NYI')
		end
		coroutine.yield(enumdecl)
		local subscope = copy(scope)
		if enumdecl.enumclass then
			subscope.enumclass = enumdecl
			if not enumdecl.name then
				lerror('enum name is required')
			end
			for i, part in ipairs(enumdecl.name) do
				table.append(subscope, part)
			end
		else
			subscope.enumtype = enumdecl
		end
		if token.punctuator ~= '{' then
			lerror('{ expected')
		end
		advance()
		readenumregion(subscope)
		if token.punctuator ~= '}' then
			lerror('} expected')
		end
		advance()
		if token.punctuator ~= ';' then
			lerror('; expected')
		end
		advance()
	end

	local function readnamespace(scope)
		local nsdecl = {
			namespace = true,
			location = token.location,
			scope = scope}
		advance()
		local subscope = copy(scope)
		subscope.namespace = nsdecl
		if token.ident then
			table.append(subscope, token.ident)
			nsdecl.name = {token.ident}
			coroutine.yield(nsdecl)
			advance()
		end
		if token.punctuator ~= '{' then
			lerror('{ expected')
		end
		advance()
		readdeclregion(subscope)
		if token.punctuator ~= '}' then
			lerror('} expected')
		end
		advance()
	end

	--[[local]] function readdeclregion(scope)
		while token and token.punctuator ~= '}' do
			if token.include then
				coroutine.yield({
					include = token.include,
					location = token.location})
				advance()
			else
				local attrs = readattrs()
				local specs = readdeclspecifiers()
				if token.word == 'struct' or token.word == 'class' then
					readclass(scope, specs)
				elseif token.word == 'enum' then
					readenum(scope, specs)
				elseif scope.namespace and token.word == 'namespace' then
					readnamespace(scope, specs)
				elseif scope.class and
					(token.word == 'private' or
					token.word == 'protected' or
					token.word == 'public')
				then
					advance()
					if token.punctuator ~= ':' then
						lerror(': expected')
					end
					advance()
				elseif token.remit then
					coroutine.yield({
						remit = token.remit,
						location = token.location,
						payload = token.payload})
					advance()
				else
					readdeclaration(scope, attrs, specs)
				end
			end
		end
	end

	--[[local]] function readenumregion(scope)
		local nextvalue = 0
		while token and token.punctuator ~= '}' do
			if token.ident then
				local constdecl = {
					enumconst = true,
					location = token.location,
					scope = scope,
					name = {token.ident}}
				advance()
				if token.punctuator == '=' then
					advance()
					constdecl.value = readnumber()
				else
					constdecl.value = nextvalue
				end
				nextvalue = constdecl.value + 1
				coroutine.yield(constdecl)
				if token.punctuator == ',' then
					advance()
				else
					break
				end
			end
		end
	end

	readdeclregion({namespace = true})
	if token then
		lerror('declaration expected')
	end
end

local function parser(path)
	return stage(parser_main, path)
end



local function attrdefstring(attrs)
	if not attrs then
		return ''
	end
	local attrparts = {}
	for name, params in pairs(attrs) do
		local attrpart = name
		if params then
			local paramparts = {}
			for j, param in ipairs(params) do
				if type(param) == 'string' then
					paramparts[j] = string.format('%q', param)
				else
					paramparts[j] = '/*unknown*/ 0'
				end
			end
			attrpart = attrpart .. '(' .. table.concat(paramparts, ', ') .. ')'
		end
		table.append(attrparts, ' [[' .. attrpart .. ']]')
	end
	return table.concat(attrparts)
end

local function typedefstring(decltype)
	local function cvstr()
		local part = ''
		if decltype.isconst then
			part = part .. 'const '
		end
		if decltype.isvolatile then
			part = part .. 'volatile '
		end
		if decltype.isnoexcept then
			part = part .. 'noexcept '
		end
		return part
	end

	local str = ''

	while true do
		if decltype.fundamental then
			return str .. cvstr() .. decltype.fundamental
		elseif decltype.name then
			return str .. cvstr() .. namestring(decltype.name)
		elseif decltype.template then
			local argstr = {}
			for i, arg in ipairs(decltype.args) do
				argstr[i] = typedefstring(arg)
			end
			return str .. cvstr() .. namestring(decltype.template) ..
				'<' .. table.concat(argstr, ', ') .. '>'
		elseif decltype.pointer then
			str = str .. cvstr() .. 'pointer '
			decltype = decltype.pointer
		elseif decltype.lref then
			str = str .. 'ref '
			decltype = decltype.lref
		elseif decltype.rref then
			str = str .. 'xref '
			decltype = decltype.rref
		elseif decltype.array then
			if decltype.size then
				str = str .. 'array [' .. tostring(decltype.size) .. '] '
			else
				str = str .. 'array [] '
			end
			decltype = decltype.array
		elseif decltype.functionret then
			local argstr = {}
			for i, arg in ipairs(decltype.args) do
				argstr[i] = ': ' .. typedefstring(arg.type)
				if arg.name then
					argstr[i] = namestring(arg.name) .. argstr[i]
				end
				argstr[i] = argstr[i] .. attrdefstring(arg.attrs)
			end
			if decltype.convention then
				str = str .. ' ' .. decltype.convention
			end
			str = str .. cvstr() ..
				'function [' .. table.concat(argstr, ', ') .. ']: '
			decltype = decltype.functionret
		else
			return str .. '<unknown>'
		end
	end
end

local function decldefstring(decl)
	if decl.include then
		return string.format('#include <%s>',
			decl.include)
	elseif decl.namespace then
		return string.format('%s> %s = namespace%s',
			namestring(decl.scope),
			namestring(decl.name),
			attrdefstring(decl.attrs))
	elseif decl.typedef then
		return string.format('%s> %s = type %s%s',
			namestring(decl.scope),
			namestring(decl.name),
			typedefstring(decl.type),
			attrdefstring(decl.attrs))
	elseif decl.class then
		local bases = {}
		for i, base in ipairs(decl.bases) do
			bases[i] = namestring(base)
		end
		return string.format('%s> %s = class(%s)%s',
			namestring(decl.scope),
			namestring(decl.name),
			table.concat(bases, ', '),
			attrdefstring(decl.attrs))
	elseif decl.enumtype then
		return string.format('%s> %s = enum%s',
			namestring(decl.scope),
			namestring(decl.name),
			attrdefstring(decl.attrs))
	elseif decl.enumclass then
		return string.format('%s> %s = enum class%s',
			namestring(decl.scope),
			namestring(decl.name),
			attrdefstring(decl.attrs))
	elseif decl.enumconst then
		return string.format('%s> %s = enum const %i%s',
			namestring(decl.scope),
			namestring(decl.name),
			decl.value,
			attrdefstring(decl.attrs))
	elseif decl.object then
		return string.format('%s> %s = %s%sobject %s%s',
			namestring(decl.scope),
			namestring(decl.name),
			decl.isfriend and 'friend ' or '',
			decl.isstatic and 'static ' or '',
			typedefstring(decl.type),
			attrdefstring(decl.attrs))
	elseif decl.remit then
		return decl.remit
	else
		return 'unknown(' .. locationstring(decl.location) .. ')'
	end
end



if not modname then
	local source = parser('utils/encoding.hpp')
	while true do
		local decl = source()
		if not decl then
			break
		end
		print(decldefstring(decl))
	end
end

parserlib.locationstring = locationstring
parserlib.locationerror = locationerror
parserlib.parser = parser
parserlib.typedefstring = typedefstring
parserlib.decldefstring = decldefstring
