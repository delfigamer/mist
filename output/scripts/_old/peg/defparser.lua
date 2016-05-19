local modname = ...
module(modname, package.seeall)
local utility = require('base.utility')
local peg = package.modulepack{
	prefix = 'peg.',
	names = {
		'andpred',
		'choice',
		'concat',
		'constant',
		'failure',
		'flatseq',
		'frame',
		'oneormore',
		'optional',
		'notpred',
		'prioritychoice',
		'proxy',
		'sequence',
		'structure',
		'terminal',
		'zeroormore'}}

local function crange2(fc, lc, t)
	t = t or {}
	local fi, li = string.byte(fc), string.byte(lc)
	for i = fi, li do
		t[string.char(i)] = true
	end
end

local function crange(t)
	local result = {}
	for i, te in ipairs(t) do
		if #te == 2 then
			crange2(te:sub(1,1), te:sub(2,2), result)
		else
			result[te] = true
		end
	end
	return result
end

local function const(str)
	return peg.constant:create{str}
end

local function assertion(rule, errmsg)
	local err = peg.failure:create{
		errmsg or string.format('%s expected', tostring(rule)),
		name = rule.name}
	return peg.choice:create{rule, err}
end

local function iconst(str)
	return assertion(const(str))
end

anychar = peg.terminal:create{
	utility.retriever(true),
	name = 'anychar'}
letter = peg.terminal:create{
	crange{'09', 'AZ', 'az', '_', '.'},
	name = 'letter'}
digit = peg.terminal:create{
	crange{'09'},
	name = digit}
hexdigit = peg.terminal:create{
	crange{'09', 'AF', 'af'},
	name = 'hexdigit'}
whitespace = peg.terminal:create{
	crange{'\0 '},
	name = 'whitespace'}
comment_line = peg.sequence:create{
	const'//',
	peg.zeroormore:create{
		peg.terminal:create{function(c) return c ~= '\n' end}},
	name = 'comment_line'}
comment_ml_proxy = peg.proxy:create{}
comment_ml = peg.sequence:create{
	const'/*',
	peg.zeroormore:create{
		peg.choice:create{
			comment_ml_proxy,
			peg.frame:create{
				left = peg.notpred:create{const'*/'},
				center = anychar}}},
	const'*/',
	name = 'comment_ml'}
comment_ml_proxy.rule = comment_ml
blank = peg.zeroormore:create{
	peg.choice:create{
		whitespace,
		comment_line,
		comment_ml},
	name = 'blank'}
number = peg.concat:create{
	peg.flatseq:create{
		peg.optional:create{const'-'},
		peg.oneormore:create{digit}},
	name = 'number'}
identifier_argref = peg.structure:create{
	rules = {const'$narg', blank, assertion(number)},
	names = {nil, nil, 'index'},
	name = 'identifier_argref'}
identifier = peg.choice:create{
	peg.concat:create{
		peg.oneormore:create{
			letter}},
	identifier_argref,
	name = 'identifier'}
str_char = peg.terminal:create{
	function(c) return c ~= '\'' end,
	name = 'str_char'}
str_chunk = peg.concat:create{
	peg.zeroormore:create{
		str_char},
	name = 'str_chunk'}
str_content = peg.flatseq:create{
	str_chunk,
	peg.zeroormore:create{
		peg.frame:create{
			left = const'\'\'',
			center = str_chunk}},
	name = 'str_content'}
str_literal = peg.structure:create{
	rules = {const'\'', str_content, iconst'\''},
	names = {nil, 'content', nil},
	name = 'str_literal'}
str = peg.choice:create{
	str_literal,
	name = 'str'}
charclasshexchar = peg.structure:create{
	rules = {const'\\', hexdigit, hexdigit},
	names = {nil, 'hi', 'lo'},
	name = 'charclasshexchar'}
charclassescapechar = peg.structure:create{
	rules = {const'\\', anychar},
	names = {nil, 'char'},
	name = 'charclassescapechar'}
charclassplainchar_b = peg.terminal:create{
	function(c)
		return c ~= ']'
	end,
	name = 'charclassplainchar_b'}
charclassplainchar = peg.structure:create{
	rules = {charclassplainchar_b},
	names = {'char'},
	name = 'charclassplainchar'}
charclasschar = peg.choice:create{
	charclasshexchar, charclassescapechar, charclassplainchar,
	name = 'charclasschar'}
charclasscharelement = peg.structure:create{
	rules = {charclasschar},
	names = {'char'},
	name = 'charclasscharelement'}
charclassrangeelement = peg.structure:create{
	rules = {charclasschar, const'-', charclasschar},
	names = {'first', nil, 'last'},
	name = 'charclassrangeelement'}
charclasscontentelement = peg.choice:create{
	charclassrangeelement, charclasscharelement,
	name = 'charclasscontentelement'}
charclasscontent = peg.zeroormore:create{
	charclasscontentelement,
	name = 'charclasscontent'}
charclass = peg.structure:create{
	rules = {const'[', peg.optional:create{const'~'}, charclasscontent, iconst']'},
	names = {nil, 'invert', 'content', nil},
	name = 'charclass'}
constant = peg.structure:create{
	rules = {str},
	names = {'str'},
	name = 'constant'}
failure = peg.structure:create{
	rules = {const'$error', blank, assertion(str)},
	names = {nil, nil, 'str'},
	name = 'failure'}
identref = peg.structure:create{
	rules = {identifier},
	names = {'ident'},
	name = 'identref'}
argref = peg.structure:create{
	rules = {const'$arg', blank, assertion(number)},
	names = {nil, nil, 'index'},
	name = 'argref'}
expression_proxy = peg.proxy:create{
	name = 'expression_proxy'}
subexpression = peg.frame:create{
	left = peg.sequence:create{const'(', blank},
	center = assertion(expression_proxy),
	right = peg.sequence:create{blank, iconst')'},
	name = 'subexpression'}
structure_element_named = peg.structure:create{
	rules = {identifier, blank, const'=', blank,
		assertion(expression_proxy)},
	names = {'name', nil, nil, nil, 'rule'},
	name = 'structure_element_named'}
structure_element_nameless = peg.structure:create{
	rules = {assertion(expression_proxy)},
	names = {'rule'},
	name = 'structure_element_nameless'}
structure_element = peg.choice:create{
	structure_element_named,
	structure_element_nameless,
	name = 'structure_element'}
structure_content = peg.flatseq:create{
	assertion(structure_element),
	peg.zeroormore:create{
		peg.frame:create{
			left = peg.sequence:create{blank, const',', blank},
			center = assertion(structure_element)}},
	name = 'structure_content'}
structure = peg.structure:create{
	rules = {identifier, blank, const'{', blank,
		structure_content, blank, iconst'}'},
	names = {'name', nil, nil, nil, 'content', nil, nil},
	name = 'structure'}
any = peg.structure:create{
	rules = {const'$any'},
	names = {nil},
	name = 'any'}
eof = peg.structure:create{
	rules = {const'$eof'},
	names = {nil},
	name = 'eof'}
dummy = peg.structure:create{
	rules = {const'$dummy'},
	names = {nil},
	name = 'dummy'}
assertable = peg.choice:create{
	structure, identref, constant, argref, any, eof,
	name = 'assertable'}
assert_modifier = peg.structure:create{
	rules = {const'$assert', blank, assertion(assertable)},
	names = {nil, nil, 'rule'},
	name = 'assert_modifier'}
elementary = peg.choice:create{
	structure, identref, subexpression, constant, assert_modifier,
		argref, failure, charclass, any, eof, dummy,
	name = 'elementary'}
optional = peg.structure:create{
	rules = {const'?', blank, assertion(elementary)},
	names = {nil, nil, 'rule'},
	name = 'optional'}
zeroormore = peg.structure:create{
	rules = {const'*', blank, assertion(elementary)},
	names = {nil, nil, 'rule'},
	name = 'zeroormore'}
oneormore = peg.structure:create{
	rules = {const'%', blank, assertion(elementary)},
	names = {nil, nil, 'rule'},
	name = 'oneormore'}
funcarg_name = peg.structure:create{
	rules = {const'$name', blank, assertion(identifier)},
	names = {nil, nil, 'name'},
	name = 'funcarg_name'}
funcarg_rule = peg.structure:create{
	rules = {expression_proxy},
	names = {'rule'},
	name = 'funcarg_rule'}
funcarg = peg.choice:create{
	funcarg_name, funcarg_rule,
	name = 'funcarg'}
funccall_arglist_content = peg.flatseq:create{
	funcarg,
	peg.zeroormore:create{
		peg.frame:create{
			left = peg.sequence:create{
				blank, const',', blank},
			center = assertion(funcarg)}},
	name = 'funccall_arglist_content'}
funccall_arglist = peg.frame:create{
	left = peg.sequence:create{const'{', blank},
	center = peg.optional:create{funccall_arglist_content},
	right = peg.sequence:create{blank, iconst'}'},
	name = 'funccall_arglist'}
funccall = peg.structure:create{
	rules = {const'$call', blank, assertion(identifier), blank,
		peg.optional:create{funccall_arglist}},
	names = {nil, nil, 'name', nil,
		'arglist'},
	name = 'funccall'}
simple = peg.choice:create{
	optional, zeroormore, oneormore, funccall, elementary,
	name = 'simple'}
andpred = peg.structure:create{
	rules = {const'&', blank, assertion(elementary)},
	names = {nil, nil, 'rule'},
	name = 'andpred'}
notpred = peg.structure:create{
	rules = {const'~', blank, assertion(elementary)},
	names = {nil, nil, 'rule'},
	name = 'notpred'}
sequence_element = peg.choice:create{
	simple, andpred, notpred,
	name = 'sequence_element'}
sequence_content = peg.flatseq:create{
	sequence_element,
	peg.zeroormore:create{
		peg.frame:create{
			left = blank,
			center = sequence_element}},
	name = 'sequence_content'}
sequence = peg.structure:create{
	rules = {sequence_content},
	names = {'content'},
	name = 'sequence'}
flatseq_element = peg.choice:create{
	sequence,
	name = 'flatseq_element'}
flatseq_content = peg.flatseq:create{
	flatseq_element,
	peg.zeroormore:create{
		peg.frame:create{
			left = peg.sequence:create{blank, const':', blank},
			center = assertion(flatseq_element)}},
	name = 'flatseq_content'}
flatseq = peg.structure:create{
	rules = {flatseq_content},
	names = {'content'},
	name = 'flatseq'}
concat = peg.structure:create{
	rules = {const'$concat', blank, assertion(flatseq)},
	names = {nil, nil, 'rule'},
	name = 'concat'}
frame_element = peg.choice:create{
	flatseq, concat,
	name = 'frame_element'}
frame_l = peg.structure:create{
	rules = {frame_element, blank, const'<', blank,
		assertion(frame_element)},
	names = {'left', nil, nil, nil, 'center'},
	name = 'frame'}
frame_r = peg.structure:create{
	rules = {frame_element, blank, const'>', blank,
		assertion(frame_element)},
	names = {'center', nil, nil, nil, 'right'},
	name = 'frame'}
frame_lr = peg.structure:create{
	rules = {frame_element, blank, const'<', blank,
		assertion(frame_element), blank, const'>', blank,
		assertion(frame_element)},
	names = {'left', nil, nil, nil, 'center', nil, nil, nil,
		'right'},
	name = 'frame'}
frame = peg.choice:create{
	frame_lr, frame_l, frame_r,
	name = 'frame_choice'}
choice_element = peg.choice:create{
	concat, frame, flatseq,
	name = 'choice_element'}
choice_content = peg.flatseq:create{
	choice_element,
	peg.zeroormore:create{
		peg.frame:create{
			left = peg.sequence:create{blank, const'/', blank},
			center = assertion(choice_element)}},
	name = 'choice_content'}
choice = peg.structure:create{
	rules = {choice_content},
	names = {'content'},
	name = 'choice'}
expression = peg.choice:create{
	choice,
	name = 'expression'}
expression_proxy.rule = expression
assignment = peg.structure:create{
	rules = {identifier, blank, const':=', blank,
		assertion(expression), blank, iconst';'},
	names = {'target', nil, nil, nil, 'expr', nil, nil},
	name = 'assignment'}
structure_definition = peg.structure:create{
	rules = {identifier, blank, const'{', blank,
		structure_content, blank, iconst'}', blank, iconst';'},
	names = {'name', nil, nil, nil, 'content', nil, nil, nil, nil},
	name = 'structure_definition'}
prioritychoice_definition = peg.structure:create{
	rules = {const'$pr', blank, identifier, blank, iconst';'},
	names = {nil, nil, 'name', nil, nil},
	name = 'prioritychoice_definition'}
insertion = peg.structure:create{
	rules = {identifier, blank, const'<', blank,
		peg.optional:create{number}, blank, iconst'=', blank,
		assertion(expression), blank, iconst';'},
	names = {'target', nil, nil, nil,
		'order', nil, nil, nil,
		'expr', nil, nil},
	name = 'insertion'}
body_proxy = peg.proxy:create{
	name = 'body_proxy'}
funcdef = peg.structure:create{
	rules = {const'$function', blank, assertion(identifier), blank,
		iconst';', blank, body_proxy, blank, iconst'$end', blank,
		iconst';'},
	names = {nil, nil, 'name', nil,
		nil, nil, 'body', nil, nil, nil,
		nil},
	name = 'funcdef'}
returnstat = peg.structure:create{
	rules = {const'$return', blank, assertion(expression), blank,
		iconst';'},
	names = {nil, nil, 'expr', nil, nil},
	name = 'returnstat'}
emptystatement = peg.structure:create{
	rules = {blank, const';'},
	names = {nil, nil},
	name = 'emptystatement'}
statement = peg.choice:create{
	assignment, structure_definition, prioritychoice_definition,
		insertion, funcdef, emptystatement,
	name = 'statement'}
statement_last = peg.choice:create{
	returnstat,
	name = 'statement_last'}
body_content = peg.flatseq:create{
	peg.zeroormore:create{
		peg.frame:create{
			center = statement,
			right = blank}},
	peg.optional:create{
		statement_last},
	name = 'body_content'}
body = peg.structure:create{
	rules = {body_content},
	names = {'content'},
	name = 'body'}
body_proxy.rule = body
file = peg.structure:create{
	rules = {blank, body, blank,
		-- assertion(peg.notpred:create{anychar},
			-- 'end of file expected'),
	},
	names = {nil, 'body', nil, nil},
	name = 'file'}
