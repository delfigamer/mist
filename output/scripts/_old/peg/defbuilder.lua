local modname = ...
module(modname, package.seeall)
local utility = require('base.utility')
local peg = package.modulepack{
	prefix = 'peg.',
	names = {
		'andpred',
		'binary',
		'choice',
		'concat',
		'constant',
		'dummy',
		'failure',
		'flatseq',
		'frame',
		'oneormore',
		'optional',
		'marker',
		'notpred',
		'prioritychoice',
		'proxy',
		'sequence',
		'structure',
		'terminal',
		'zeroormore'}}

local function crange2(fi, li)
	local t = {}
	for i = fi, li do
		t[string.char(i)] = true
	end
	return t
end

local function merge(ts)
	local result = {}
	for i, te in ipairs(ts) do
		for k, v in pairs(te) do
			result[k] = v
		end
	end
	return result
end

function derivenamespace(parent)
	local new = setmetatable(
			{},
			{__index = parent})
	return new
end

function retrieve(context, ident, def)
	return assert(
		context.namespace[ident],
		string.format('[%i:%i] %s is undefined',
			def.row, def.col, ident))
end

function assign(context, ident, obj, def)
	if rawget(context.namespace, ident) then
		error(string.format('[%i:%i] %s is already defined',
			def.row, def.col, ident))
	end
	context.namespace[ident] = obj
end

function retrievearg(context, index, def)
	index = tonumber(index)
	local arglist = assert(context.arglist,
		string.format('[%i:%i] this place is not in a function',
			def.row, def.col))
	return assert(arglist[index],
		string.format('[%i:%i] argument %i is undefined',
			def.row, def.col, index)), index
end

function build(def, ...)
	if type(def) == 'table' and def.name then
		local func = _M['build_' .. def.name]
		if func then
			return func(def, ...)
		else
			return def
		end
	else
		return def
	end
end

function build_identifier_argref(def, context)
	local arg, index = retrievearg(context, def._index, def)
	return assert(arg.def_name,
		string.format('[%i:%i] argument %i is not a name',
			def.row, def.col, index))
end

function build_str_literal(def, context)
	return table.concat(def._content, '\'')
end

local hextable = {
	['0'] = 0, ['8'] = 8,
	['1'] = 1, ['9'] = 9,
	['2'] = 2, ['a'] = 10, ['A'] = 10,
	['3'] = 3, ['b'] = 11, ['B'] = 11,
	['4'] = 4, ['c'] = 12, ['C'] = 12,
	['5'] = 5, ['d'] = 13, ['D'] = 13,
	['6'] = 6, ['e'] = 14, ['E'] = 14,
	['7'] = 7, ['f'] = 15, ['F'] = 15}

function build_charclasshexchar(def, context)
	return hextable[def._hi] * 16 + hextable[def._lo]
end

function build_charclassescapechar(def, context)
	return string.byte(def._char)
end

function build_charclassplainchar(def, context)
	return string.byte(def._char)
end

function build_charclasscharelement(def, context)
	local char = build(def._char, context)
	return {[string.char(char)] = true}
end

function build_charclassrangeelement(def, context)
	local first = build(def._first, context)
	local last = build(def._last, context)
	return crange2(first, last)
end

function build_charclasscontent(def, context)
	local t = {}
	for i, e in ipairs(def) do
		t[i] = build(e, context)
	end
	return merge(t)
end

function build_charclass(def, context)
	local content = build_charclasscontent(def._content, context)
	if def._invert then
		local inv = {}
		for i = 0, 255 do
			local ch = string.char(i)
			if not content[ch] then
				inv[ch] = true
			end
		end
		content = inv
	end
	return peg.terminal:create{
		content,
		scopeprefix = context.scopeprefix}
end

function build_constant(def, context)
	return peg.constant:create{
		build(def._str, context),
		scopeprefix = context.scopeprefix}
end

function build_failure(def, context)
	return peg.failure:create{
		build(def._str, context),
		scopeprefix = context.scopeprefix}
end

function build_identref(def, context)
	local ident = build(def._ident, context)
	local proxy = peg.proxy:create{
		name = ident,
		scopeprefix = context.scopeprefix}
	local function link()
		local target = retrieve(context, ident, def)
		assert(
			type(target) == 'table' and target.def_rule,
			string.format('[%i:%i] %s is not a rule',
				def.row, def.col, ident))
--		print(string.format('[%i:%i] linking %s, namespace is %s' ..
--			'\n\tresult is %s',
--			def.row, def.col, ident, tostring(context.namespace),
--			target.def_rule:defstring()))
		proxy.rule = target.def_rule
	end
	context.postprocess[#context.postprocess + 1] = link
	return proxy
end

function build_argref(def, context)
	local arg, index = retrievearg(context, def._index, def)
	return assert(arg.def_rule,
		string.format('[%i:%i] argument %i is not a rule',
			def.row, def.col, index))
end

function build_structure(def, context)
	local name = build(def._name, context)
	local it = {
		rules = {},
		names = {},
		name = name,
		scopeprefix = context.scopeprefix}
	for i, elem in ipairs(def._content) do
		it.rules[i] = build(elem._rule, context)
		it.names[i] = build(elem._name, context)
	end
	return peg.structure:create(it)
end

function build_any(def, context)
	return peg.terminal:create{
		utility.operator.identity,
		transform = true,
		name = '$any',
		scopeprefix = context.scopeprefix}
end

function build_eof(def, context)
	return peg.terminal:create{
		utility.operator.lnot,
		transform = true,
		name = '$eof',
		scopeprefix = context.scopeprefix}
end

function build_dummy(def, context)
	return peg.dummy:create{
		scopeprefix = context.scopeprefix}
end

function build_marker(def, context)
	return peg.marker:create{
		build(def._str, context),
		scopeprefix = context.scopeprefix}
end

function build_assert_modifier(def, context)
	local rule = build(def._rule, context)
	return peg.choice:create{
		rule,
		peg.failure:create{
			rule.name .. ' expected'},
		name = '$assert ' .. rule.name,
		scopeprefix = context.scopeprefix}
end

function build_optional(def, context)
	local rule = build(def._rule, context)
	return peg.optional:create{
		rule,
		scopeprefix = context.scopeprefix}
end

function build_zeroormore(def, context)
	local rule = build(def._rule, context)
	return peg.zeroormore:create{
		rule,
		scopeprefix = context.scopeprefix}
end

function build_oneormore(def, context)
	local rule = build(def._rule, context)
	return peg.oneormore:create{
		rule,
		scopeprefix = context.scopeprefix}
end

function build_funcarg_name(def, context)
	return {
		def_name = build(def._name, context)}
end

function build_funcarg_rule(def, context)
	return {
		def_rule = build(def._rule, context)}
end

function build_funccall(def, context)
	local name = build(def._name, context)
	local func = retrieve(context, name, def)
	func = assert(
		type(func) == 'table' and func.def_func,
		string.format('[%i:%i] %s is not a function',
			def.row, def.col, name))
	local arglist = {}
	if def._arglist then
		for i, argdef in ipairs(def._arglist) do
			arglist[i] = build(argdef, context)
		end
	end
	local scopeprefix = string.format('%s%s::',
		context.scopeprefix or '', name)
	local newcontext = {
		postprocess = context.postprocess,
		namespace = derivenamespace(context.namespace),
		arglist = arglist,
		scopeprefix = scopeprefix}
--	print(string.format('[%i:%i] calling %s, namespace is %s',
--			def.row, def.col, name, tostring(newcontext.namespace)))
	build(func.body, newcontext)
	return assert(newcontext.result,
		string.format('[%i:%i] %s does not return a rule',
			def.row, def.col, name))
end

function build_andpred(def, context)
	local rule = build(def._rule, context)
	return peg.andpred:create{
		rule,
		scopeprefix = context.scopeprefix}
end

function build_notpred(def, context)
	local rule = build(def._rule, context)
	return peg.notpred:create{
		rule,
		scopeprefix = context.scopeprefix}
end

function build_sequence(def, context)
	local it = {
		scopeprefix = context.scopeprefix}
	for i, rule in ipairs(def._content) do
		it[i] = build(rule, context)
	end
	if #it == 1 then
		return it[1]
	else
		return peg.sequence:create(it)
	end
end

function build_frame(def, context)
	local left = def._left and build(def._left, context)
	local center = build(def._center, context)
	local right = def._right and build(def._right, context)
	return peg.frame:create{
		left = left, center = center, right = right,
		scopeprefix = context.scopeprefix}
end

function build_flatseq(def, context)
	local it = {
		scopeprefix = context.scopeprefix}
	for i, rule in ipairs(def._content) do
		it[i] = build(rule, context)
	end
	if #it == 1 then
		return it[1]
	else
		return peg.flatseq:create(it)
	end
end

function build_concat(def, context)
	local rule = build(def._rule, context)
	return peg.concat:create{
		rule,
		scopeprefix = context.scopeprefix}
end

function build_binary(def, context)
	local name = build(def._name, context)
	local content = def._content
	local prefix = build(content._prefix, context)
	local infix = content._infix and build(content._infix, context)
	local suffix = build(content._suffix, context)
	return peg.binary:create{
		name = name,
		prefix = prefix, infix = infix, suffix = suffix,
		scopeprefix = context.scopeprefix}
end

function build_choice(def, context)
	local it = {
		scopeprefix = context.scopeprefix}
	for i, rule in ipairs(def._content) do
		it[i] = build(rule, context)
	end
	if #it == 1 then
		return it[1]
	else
		return peg.choice:create(it)
	end
end

function build_assignment(def, context)
	local target = build(def._target, context)
	local expr = {
		def_rule = build(def._expr, context)}
	assign(context, target, expr, def)
end

function build_structure_definition(def, context)
	local name = build(def._name, context)
	local sdef = {
		name = 'structure',
		_name = def._name,
		_content = def._content}
	local expr = {
		def_rule = build(sdef, context)}
	assign(context, name, expr, def)
end

function build_binary_definition(def, context)
	local name = build(def._name, context)
	local sdef = {
		name = 'binary',
		_name = def._name,
		_content = def._content}
	local expr = {
		def_rule = build(sdef, context)}
	assign(context, name, expr, def)
end

function build_prioritychoice_definition(def, context)
	local name = build(def._name, context)
	local rule = peg.prioritychoice:create{
		name = name,
		scopeprefix = context.scopeprefix}
	local expr = {
		def_rule = rule,
		def_pr = rule}
	assign(context, name, expr, def)
end

function build_insertion(def, context)
	local target = build(def._target, context)
	local pr = retrieve(context, target, def)
	assert(
		type(pr) == 'table' and pr.def_pr,
		string.format('[%i:%i] cannot insert into %s',
			def.row, def.col, target))
	local expr = build(def._expr, context)
	local order = def._order and tonumber(def._order)
	pr.def_pr:insert(expr, order)
end

function build_funcdef(def, context)
	local name = build(def._name, context)
	local func = {
		body = def._body}
	local expr = {
		def_func = func}
	assign(context, name, expr, def)
end

function build_returnstat(def, context)
	local expr = build(def._expr, context)
	context.result = expr
end

function build_body(def, context)
	for i, stat in ipairs(def._content) do
		build(stat, context)
	end
end

function build_file(def, parent)
	local context = {
		postprocess = {},
		namespace = derivenamespace(parent),
		arglist = {}}
	build(def._body, context)
	for i, func in ipairs(context.postprocess) do
		func()
	end
	return context.result, context.namespace
end
