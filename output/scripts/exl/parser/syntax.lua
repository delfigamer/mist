local modname = ...
local syntax = package.modtable(modname)
local common = require(modname, 2, 'common')

local function acquiretoken(ts, type, blinestart)
	local token = ts:gett()
	if token.type ~= type
		or blinestart ~= nil and token.bislinestart ~= blinestart
	then
		ts:error(type .. ' expected', token.spos)
	else
		return token
	end
end

local function acquirenode(ts, func, name)
	local node = func(ts)
	if not node then
		local token = ts:peekt()
		ts:error(name .. ' expected', token.spos)
	else
		return node
	end
end

local function getname(ts, lead, strict)
	local name = {}
	lead = lead or ts:gett()
	local token = lead
	if token.type == '.' then
		name.global = true
		token = ts:gett()
	end
	while token.type == 'identifier' then
		table.append(name, token.content)
		name.epos = token.epos
		token = ts:gett()
		if token.type == '.' then
			token = ts:gett()
		else
			ts:ungett(token)
		end
	end
	ts:ungett(token)
	if #name == 0 then
		if name.global or strict then
			ts:error('name expected', token.spos)
		else
			return
		end
	else
		name.spos = lead.spos
		return name
	end
end

local function selector_gen(table, default)
	return function(ts, ...)
		local lead = ts:gett()
		local func = table[lead.type]
		if func then
			return func(ts, lead, ...)
		else
			ts:ungett(lead)
			if default then
				return default(ts, ...)
			else
				return
			end
		end
	end
end

local kwset_end = {
	['end'] = true}

syntax.expr = {}

syntax.expr.element = {}

syntax.expr.element['number'] = function(ts, lead)
	return common.createnode{
		name = 'expr.number',
		spos = lead.spos,
		epos = lead.epos,
		filename = ts.filename,
		value = lead.content}
end

syntax.expr.element['string'] = function(ts, lead)
	return common.createnode{
		name = 'expr.string',
		spos = lead.spos,
		epos = lead.epos,
		filename = ts.filename,
		value = lead.content}
end

syntax.expr.element['nil'] = function(ts, lead)
	return common.createnode{
		name = 'expr.nil',
		spos = lead.spos,
		epos = lead.epos,
		filename = ts.filename}
end

syntax.expr.element['identifier'] = function(ts, lead)
	local target = getname(ts, lead)
	local token = ts:peekt()
	if token.type == '[' then
		ts:gett()
		token = ts:peekt()
		if token.type == ']' then
			ts:gett()
			return common.createnode{
				name = 'expr.invoke',
				target = lead.content,
				spos = lead.spos,
				epos = token.epos,
				filename = ts.filename,
				args = {}}
		end
		local args = {}
		while true do
			local arg = acquirenode(ts, syntax.expr.main, 'argument')
			if arg then
				table.append(args, arg)
			end
			token = ts:gett()
			if token.type == ']' then
				break
			elseif token.type ~= ',' then
				ts:error('] expected', token.spos)
			end
		end
		return common.createnode{
			name = 'expr.invoke',
			target = lead.content,
			spos = lead.spos,
			epos = token.epos,
			filename = ts.filename,
			args = args}
	else
		return common.createnode{
			name = 'expr.identifier',
			spos = lead.spos,
			epos = lead.epos,
			filename = ts.filename,
			targetname = lead.content}
	end
end

syntax.expr.element['('] = function(ts, lead)
	local value = acquirenode(ts, syntax.expr.main, 'expression')
	local et = acquiretoken(ts, ')')
	return common.createnode{
		name = 'expr.subexpression',
		spos = lead.spos,
		epos = et.epos,
		filename = ts.filename,
		value = value}
end

syntax.argdef = function(ts)
	local node, target, lvalue, rvalue, lead
	local token = ts:gett()
	lead = token
	if token.type == 'in' then
		lvalue, rvalue = false, true
		token = ts:gett()
	elseif token.type == 'out' then
		lvalue, rvalue = true, false
		token = ts:gett()
	elseif token.type == 'inout' then
		lvalue, rvalue = true, true
		token = ts:gett()
	else
		lvalue, rvalue = false, true
	end
	local targetname, value, typev
	if token.type == 'identifier' then
		targetname = token.content
		token = ts:gett()
	end
	if token.type == '=' then
		value = acquirenode(ts, syntax.expr.main, 'expression')
		token = ts:gett()
	end
	if token.type == ':' then
		typev = acquirenode(ts, syntax.expr.main, 'type')
	else
		ts:error('type expected', token.spos)
	end
	token = ts:gett()
	if token.type == ',' or token.type == ']' then
		ts:ungett(token)
		return common.createnode{
			name = 'expr.function.argdef',
			spos = lead.spos,
			epos = typev.epos,
			filename = ts.filename,
			lvalue = lvalue,
			rvalue = rvalue,
			target = targetname,
			value = value,
			typev = typev}
	else
		ts:error('] expected', nt.spos)
	end
end

function syntax.argdeflist(ts)
	local lead = acquiretoken(ts, '[', false)
	local nt = ts:peekt()
	if nt.type == ']' then
		ts:gett()
		return {}
	end
	local argdefs = {}
	while true do
		local argdef = syntax.argdef(ts)
		table.append(argdefs, argdef)
		nt = ts:gett()
		if nt.type == ']' then
			break
		elseif nt.type ~= ',' then
			ts:error('] expected', nt.spos)
		end
	end
	return argdefs
end

syntax.expr.element['function'] = function(ts, lead)
	local argdefs = acquirenode(ts, syntax.argdeflist, 'arglist')
	local rettype
	local token = ts:gett()
	if token.type == ':' then
		rettype = acquirenode(ts, syntax.expr.main, 'return type')
	else
		ts:ungett(token)
	end
	local body = syntax.block(ts, 'block', syntax.stat.main, kwset_end)
	token = ts:gett()
	return common.createnode{
		name = 'expr.function',
		spos = lead.spos,
		epos = token.epos,
		filename = ts.filename,
		argdefs = argdefs,
		rettype = rettype,
		body = body}
end

syntax.expr.type = {}

syntax.expr.type['function'] = function(ts, nlead, lead)
	local argdefs = acquirenode(ts, syntax.argdeflist, 'argument list')
	local rettype
	local token = ts:gett()
	if token.type == ':' then
		rettype = acquirenode(ts, syntax.expr.main, 'return type')
	else
		ts:ungett(token)
	end
	return common.createnode{
		name = 'expr.function.typev',
		spos = lead.spos,
		epos = rettype.epos,
		filename = ts.filename,
		argdefs = argdefs,
		rettype = rettype}
end

syntax.expr.type.expr = function(ts, lead)
	local expr = acquirenode(ts, syntax.expr.unary, 'expression')
	return common.createnode{
		name = 'expr.typeof',
		spos = lead.spos,
		epos = expr.epos,
		filename = ts.filename,
		expression = expr}
end

syntax.expr.element['type'] = selector_gen(
	syntax.expr.type,syntax.expr.type.expr)

syntax.expr.element['main'] = selector_gen(syntax.expr.element)

local unaryname = {
	['+'] = 'plus',
	['-'] = 'minus'}

syntax.expr.unary = function(ts)
	local prefixes = {}
	while true do
		local sign = ts:gett()
		if not unaryname[sign.type] then
			ts:ungett(sign)
			break
		end
		table.append(prefixes, sign)
	end
	if not prefixes[1] then
		return syntax.expr.element.main(ts)
	end
	local result = acquirenode(ts, syntax.expr.element.main, 'element')
	for i = #prefixes, 1, -1 do
		local sign = prefixes[i]
		result = common.createnode{
			name = 'expr.invoke',
			target = unaryname[sign.type],
			spos = sign.spos,
			epos = result.epos,
			filename = ts.filename,
			args = {result}}
	end
	return result
end

syntax.expr.suffix = {}

syntax.expr.suffix['['] = function(ts, lead, base)
	-- if lead.bislinestart then
		-- return
	-- end
	local token = ts:peekt()
	if token.type == ']' then
		ts:gett()
		return common.createnode{
			name = 'expr.invoke',
			target = 'invoke',
			spos = base.spos,
			epos = token.epos,
			filename = ts.filename,
			args = {base}}
	end
	local args = {base}
	while true do
		local arg = acquirenode(ts, syntax.expr.main, 'argument')
		if arg then
			table.append(args, arg)
		end
		token = ts:gett()
		if token.type == ']' then
			break
		elseif token.type ~= ',' then
			ts:error('] expected', token.spos)
		end
	end
	return common.createnode{
		name = 'expr.invoke',
		target = 'invoke',
		spos = base.spos,
		epos = token.epos,
		filename = ts.filename,
		args = args}
end

-- syntax.expr.suffix['.'] = function(ts, lead, base)
	-- local index = acquiretoken(ts, 'identifier')
	-- return common.createnode{
		-- name = 'expr.scope',
		-- spos = base.spos,
		-- epos = index.epos,
		-- filename = ts.filename,
		-- base = base,
		-- index = index.content,
	-- }
-- end

syntax.expr.suffix['as'] = function(ts, lead, base)
	local token = ts:gett()
	local lvalue, rvalue
	if token.type == 'in' then
		lvalue, rvalue = false, true
	elseif token.type == 'out' then
		lvalue, rvalue = true, false
	elseif token.type == 'inout' then
		lvalue, rvalue = true, true
	else
		lvalue, rvalue = false, true
		ts:ungett(token)
	end
	local typev = acquirenode(ts, syntax.expr.unary, 'identifier')
	return common.createnode{
		name = 'expr.cast',
		spos = base.spos,
		epos = typev.epos,
		filename = ts.filename,
		base = base,
		typev = typev,
		blvalue = lvalue,
		brvalue = rvalue,
		bexplicit = true,
	}
end

syntax.expr.suffix.main = function(ts)
	local base = syntax.expr.unary(ts)
	if not base then
		return
	end
	while true do
		local token = ts:gett()
		local func = syntax.expr.suffix[token.type]
		if func then
			local value = func(ts, token, base)
			if value then
				base = value
			else
				ts:ungett(token)
				return base
			end
		else
			ts:ungett(token)
			return base
		end
	end
end

local binaryname = {
	['..'] = 'join',

	['*'] = 'times',
	['/'] = 'divide',

	['+'] = 'plus',
	['-'] = 'subtract',

	['=='] = 'equal',
	['~='] = 'unequal',

	['='] = 'assign',
	['+='] = 'addto',
	['-='] = 'subtractfrom',
}

local function binary_gen(first, next, ...)
	local set = table.makeset{...}
	return function(ts)
		local result = first(ts)
		if not result then
			return
		end
		while true do
			local sign = ts:gett()
			if not set[sign.type] then
				ts:ungett(sign)
				break
			end
			local rs = acquirenode(ts, next, 'element')
			result = common.createnode{
				name = 'expr.invoke',
				target = binaryname[sign.type],
				spos = result.spos,
				epos = rs.epos,
				filename = ts.filename,
				args = {result, rs}}
		end
		return result
	end
end

local function rbinary_gen(first, next, ...)
	local set = table.makeset{...}
	return function(ts)
		local elist = {first(ts)}
		if not elist[1] then
			return
		end
		local slist = {}
		while true do
			local sign = ts:gett()
			if not set[sign.type] then
				ts:ungett(sign)
				break
			end
			local rs = acquirenode(ts, next, 'element')
			local i = table.append(elist, rs)
			slist[i-1] = sign.type
		end
		local result = elist[#elist]
		for i = #elist-1, 1, -1 do
			local ls = elist[i]
			result = common.createnode{
				name = 'expr.invoke',
				target = binaryname[slist[i]],
				spos = ls.spos,
				epos = result.epos,
				filename = ts.filename,
				args = {ls, result}}
		end
		return result
	end
end

syntax.expr.concat = binary_gen(
	syntax.expr.suffix.main, syntax.expr.suffix.main, '..')
syntax.expr.prod = binary_gen(
	syntax.expr.concat, syntax.expr.concat, '*', '/')
syntax.expr.sum = binary_gen(
	syntax.expr.prod, syntax.expr.prod, '+', '-')
syntax.expr.assign = rbinary_gen(
	syntax.expr.sum, syntax.expr.sum, '=', '+=', '-=')

function syntax.expr.main(ts)
	local node = syntax.expr.assign(ts)
	return node
end

syntax.stat = {}

-- syntax.stat['class'] = function(ts, lead)
	-- local classname = acquiretoken(ts, 'identifier')
	-- local parent
	-- local token = ts:gett()
	-- if token.type == ':' then
		-- parent = acquirenode(ts, syntax.expr.main, 'parent')
	-- else
		-- ts:ungett(token)
	-- end
	-- local body = syntax.block(
		-- ts, 'class.body', syntax.class.member.main, kwset_end)
	-- token = ts:gett()
	-- return common.createnode{
		-- name = 'stat.class',
		-- spos = lead.spos,
		-- epos = token.epos,
		-- filename = ts.filename,
		-- classname = classname.content,
		-- parent = parent,
		-- body = body,
	-- }
-- end

syntax.stat['const'] = function(ts, lead)
	local targetname = acquiretoken(ts, 'identifier')
	local sign = acquiretoken(ts, '=')
	local value = acquirenode(ts, syntax.expr.main, 'expression')
	return common.createnode{
		name = 'stat.const',
		spos = lead.spos,
		epos = value.epos,
		filename = ts.filename,
		targetname = targetname.content,
		value = value}
end

syntax.stat['function'] = function(ts, lead)
	local targetname = acquiretoken(ts, 'identifier')
	local argdefs = acquirenode(ts, syntax.argdeflist, 'arglist')
	local rettype
	local token = ts:gett()
	if token.type == ':' then
		rettype = acquirenode(ts, syntax.expr.main, 'return type')
	else
		ts:ungett(token)
	end
	local body = syntax.block(ts, 'block', syntax.stat.main, kwset_end)
	token = ts:gett()
	return common.createnode{
		name = 'stat.function',
		spos = lead.spos,
		epos = token.epos,
		filename = ts.filename,
		targetname = targetname.content,
		argdefs = argdefs,
		rettype = rettype,
		body = body}
end

syntax.stat['local'] = function(ts, lead)
	local targetname = acquiretoken(ts, 'identifier')
	local sign = ts:gett()
	if sign.type == '=' then
		local value = acquirenode(ts, syntax.expr.main, 'expression')
		sign = ts:gett()
		if sign.type == ':' then
		local typev = acquirenode(ts, syntax.expr.main, 'type')
			return common.createnode{
				name = 'stat.local',
				spos = lead.spos,
				epos = value.epos,
				filename = ts.filename,
				targetname = targetname.content,
				typev = typev,
				value = value}
		else
			ts:ungett(sign)
			return common.createnode{
				name = 'stat.local',
				spos = lead.spos,
				epos = value.epos,
				filename = ts.filename,
				targetname = targetname.content,
				value = value}
		end
	elseif sign.type == ':' then
		local typev = acquirenode(ts, syntax.expr.main, 'type')
		return common.createnode{
			name = 'stat.local',
			spos = lead.spos,
			epos = typev.epos,
			filename = ts.filename,
			targetname = targetname.content,
			typev = typev}
	else
		ts:error('type or initializer expected', sign.spos)
	end
end

function syntax.stat.expr(ts)
	local value = syntax.expr.main(ts)
	if not value then
		return
	end
	return common.createnode{
		name = 'stat.expression',
		spos = value.spos,
		epos = value.epos,
		filename = ts.filename,
		value = value}
end

syntax.stat.main = selector_gen(syntax.stat, syntax.stat.expr)

function syntax.block(ts, nodename, statparser, termtypes)
	local token = ts:peekt()
	local spos = token.spos
	local statements = {}
	while true do
		token = ts:peekt()
		local ttype = token.type
		if ttype == 'eof' then
			ts:error('block end expected', token.spos)
		elseif termtypes[ttype] then
			break
		else
			local stat = acquirenode(ts, statparser, 'statement')
			table.append(statements, stat)
		end
	end
	return common.createnode{
		name = nodename,
		spos = spos,
		epos = token.spos,
		filename = ts.filename,
		statements = statements}
end

-- syntax.class = {}

-- syntax.class.member = {}

-- syntax.class.member['const'] = function(ts, lead)
	-- local targetname = acquiretoken(ts, 'identifier')
	-- local sign = acquiretoken(ts, '=')
	-- local value = acquirenode(ts, syntax.expr.main, 'expression')
	-- return common.createnode{
		-- name = 'class.member.const',
		-- spos = lead.spos,
		-- epos = targetname.epos,
		-- filename = ts.filename,
		-- targetname = targetname.content,
		-- value = value,
	-- }
-- end

-- syntax.class.member['local'] = function(ts, lead)
	-- local typev = acquirenode(ts, syntax.expr.main, 'type')
	-- local targetname = acquiretoken(ts, 'identifier')
	-- return common.createnode{
		-- name = 'class.member.local',
		-- spos = lead.spos,
		-- epos = targetname.epos,
		-- filename = ts.filename,
		-- typev = typev,
		-- targetname = targetname.content,
	-- }
-- end

-- syntax.class.member.main = selector_gen(syntax.class.member)
