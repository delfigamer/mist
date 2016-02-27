local modname = ...
local syntax = package.modtable(modname)
local common = require(modname, 2, 'common')

local createnode = common.createnode

local function acquiretoken(ts, type, blinestart)
	local token = ts:gett()
	if token:gettype() ~= type
		or blinestart ~= nil and token:islinestart() ~= blinestart
	then
		ts:error(type..' expected', token:getspos(), token:getepos())
	else
		return token
	end
end

local function acquirenode(ts, func, name)
	local node = func(ts)
	if not node then
		local token = ts:peekt()
		ts:error(name..' expected', token:getspos())
	else
		return node
	end
end

local function selector_gen(table, default)
	return function(ts, ...)
		local lead = ts:gett()
		local func = table[lead:gettype()]
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
	['end'] = true,
}

syntax.expr = {}

syntax.expr.element = {}

syntax.expr.element['number'] = function(ts, lead)
	return createnode{
		name = 'expr.number',
		spos = lead:getspos(),
		epos = lead:getepos(),
		filename = ts.filename,
		value = lead:getcontent(),
	}
end

syntax.expr.element['string'] = function(ts, lead)
	return createnode{
		name = 'expr.string',
		spos = lead:getspos(),
		epos = lead:getepos(),
		filename = ts.filename,
		value = lead:getcontent(),
	}
end

syntax.expr.element['nil'] = function(ts, lead)
	return createnode{
		name = 'expr.nil',
		spos = lead:getspos(),
		epos = lead:getepos(),
		filename = ts.filename,
	}
end

syntax.expr.element['identifier'] = function(ts, lead)
	return createnode{
		name = 'expr.identifier',
		spos = lead:getspos(),
		epos = lead:getepos(),
		filename = ts.filename,
		targetname = lead:getcontent(),
	}
end

syntax.expr.element['('] = function(ts, lead)
	local value = acquirenode(ts, syntax.expr.main, 'expression')
	local et = acquiretoken(ts, ')')
	return createnode{
		name = 'expr.subexpression',
		spos = lead:getspos(),
		epos = et:getepos(),
		filename = ts.filename,
		value = value,
	}
end

syntax.farg = function(ts)
	local node, target, lvalue, rvalue
	local token = ts:gett()
	if token:gettype() == 'in' then
		lvalue, rvalue = false, true
	elseif token:gettype() == 'out' then
		lvalue, rvalue = true, false
	elseif token:gettype() == 'inout' then
		lvalue, rvalue = true, true
	else
		ts:ungett(token)
		lvalue, rvalue = false, true
	end
	local typev = acquirenode(ts, syntax.expr.main, 'type')
	target = ts:gett()
	if target:gettype() == ',' or target:gettype() == ')' then
		ts:ungett(target)
		return createnode{
			name = 'expr.function.arg',
			spos = typev.spos,
			epos = typev.epos,
			filename = ts.filename,
			lvalue = lvalue,
			rvalue = rvalue,
			typev = typev,
		}
	elseif target:gettype() == 'identifier' then
		node = createnode{
			name = 'expr.function.arg',
			spos = typev.spos,
			epos = target:getepos(),
			filename = ts.filename,
			lvalue = lvalue,
			rvalue = rvalue,
			typev = typev,
			target = target:getcontent(),
		}
		local nt = ts:gett()
		if nt:gettype() == ',' or nt:gettype() == ')' then
			ts:ungett(nt)
			return node
		else
			ts:error(') expected', nt:getspos())
		end
	else
		ts:error('identifier expected', target:getspos())
	end
end

function syntax.farglist(ts)
	local lead = acquiretoken(ts, '(', false)
	local nt = ts:peekt()
	if nt:gettype() == ')' then
		ts:gett()
		return {}
	end
	local args = {}
	while true do
		local arg = syntax.farg(ts)
		table.append(args, arg)
		nt = ts:gett()
		if nt:gettype() == ')' then
			break
		elseif nt:gettype() ~= ',' then
			ts:error(') expected', nt:getspos())
		end
	end
	return args
end

syntax.expr.element['function'] = function(ts, lead)
	local args = acquirenode(ts, syntax.farglist, 'arglist')
	local rettype
	local token = ts:gett()
	if token:gettype() == ':' then
		rettype = acquirenode(ts, syntax.expr.main, 'return type')
	else
		ts:ungett(token)
	end
	local body = syntax.block(ts, 'block', syntax.stat.main, kwset_end)
	token = ts:gett()
	return createnode{
		name = 'expr.function',
		spos = lead:getspos(),
		epos = token:getepos(),
		filename = ts.filename,
		args = args,
		rettype = rettype,
		body = body,
	}
end

syntax.expr.element['invoke'] = function(ts, lead)
	local opname = acquiretoken(ts, 'identifier')
	local token = acquiretoken(ts, '(', false)
	token = ts:peekt()
	if token:gettype() == ')' then
		ts:gett()
		return createnode{
			name = 'expr.invoke',
			opname = opname:getcontent(),
			spos = lead:getspos(),
			epos = token:getepos(),
			filename = ts.filename,
			args = {},
		}
	end
	local args = {}
	while true do
		local arg = acquirenode(ts, syntax.expr.main, 'argument')
		table.append(args, arg)
		token = ts:gett()
		if token:gettype() == ')' then
			break
		elseif token:gettype() ~= ',' then
			ts:error(') expected', token:getspos())
		end
	end
	return createnode{
		name = 'expr.invoke',
		opname = opname:getcontent(),
		spos = lead:getspos(),
		epos = token:getepos(),
		filename = ts.filename,
		args = args,
	}
end

syntax.expr.type = {}

syntax.expr.type['function'] = function(ts, nlead, lead)
	local arglist = acquirenode(ts, syntax.farglist, 'arglist')
	local rettype
	local token = ts:gett()
	if token:gettype() == ':' then
		rettype = acquirenode(ts, syntax.expr.main, 'return type')
	else
		ts:ungett(token)
	end
	if rettype then
		return createnode{
			name = 'expr.function.typev',
			spos = lead:getspos(),
			epos = rettype.epos,
			filename = ts.filename,
			args = arglist,
			rettype = rettype,
		}
	else
		return createnode{
			name = 'expr.function.typev',
			spos = lead:getspos(),
			epos = arglist.epos,
			filename = ts.filename,
			args = arglist,
		}
	end
end

syntax.expr.type.expr = function(ts, lead)
	local expr = acquirenode(ts, syntax.expr.unary, 'expression')
	return createnode{
		name = 'expr.typeof',
		spos = lead:getspos(),
		epos = expr.epos,
		filename = ts.filename,
		expression = expr,
	}
end

syntax.expr.element['type'] = selector_gen(
	syntax.expr.type, syntax.expr.type.expr)

syntax.expr.element['main'] = selector_gen(syntax.expr.element)

local unaryname = {
	['+'] = 'identity',
	['-'] = 'negate',
	['!'] = 'bnot',
}

syntax.expr.unary = function(ts)
	local prefixes = {}
	while true do
		local sign = ts:gett()
		if not unaryname[sign:gettype()] then
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
		result = createnode{
			name = 'expr.invoke',
			opname = unaryname[sign:gettype()],
			spos = sign:getspos(),
			epos = result.epos,
			filename = ts.filename,
			args = {result},
		}
	end
	return result
end

syntax.expr.suffix = {}

syntax.expr.suffix['('] = function(ts, lead, base)
	if lead:islinestart() then
		return
	end
	local token = ts:peekt()
	if token:gettype() == ')' then
		ts:gett()
		return createnode{
			name = 'expr.invoke',
			opname = 'call',
			spos = base.spos,
			epos = token:getepos(),
			filename = ts.filename,
			args = {base},
		}
	end
	local args = {base}
	while true do
		local arg = acquirenode(ts, syntax.expr.main, 'argument')
		if arg then
			table.append(args, arg)
		end
		token = ts:gett()
		if token:gettype() == ')' then
			break
		elseif token:gettype() ~= ',' then
			ts:error(') expected', token:getspos())
		end
	end
	return createnode{
		name = 'expr.invoke',
		opname = 'call',
		spos = base.spos,
		epos = token:getepos(),
		filename = ts.filename,
		args = args,
	}
end

syntax.expr.suffix['.'] = function(ts, lead, base)
	local index = acquiretoken(ts, 'identifier')
	return createnode{
		name = 'expr.scope',
		spos = base.spos,
		epos = index:getepos(),
		filename = ts.filename,
		base = base,
		index = index:getcontent(),
	}
end

syntax.expr.suffix['as'] = function(ts, lead, base)
	local token = ts:gett()
	local lvalue, rvalue
	if token:gettype() == 'in' then
		lvalue, rvalue = false, true
	elseif token:gettype() == 'out' then
		lvalue, rvalue = true, false
	elseif token:gettype() == 'inout' then
		lvalue, rvalue = true, true
	else
		lvalue, rvalue = false, true
		ts:ungett(token)
	end
	local typev = acquirenode(ts, syntax.expr.unary, 'identifier')
	return createnode{
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
		local func = syntax.expr.suffix[token:gettype()]
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
	['..'] = 'concat',

	['*'] = 'mul',
	['/'] = 'div',

	['+'] = 'add',
	['-'] = 'sub',

	['~'] = 'bxor',
	['=='] = 'eq',
	['~='] = 'neq',

	['='] = 'assign',
	['+='] = 'adda',
	['-='] = 'suba',
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
			if not set[sign:gettype()] then
				ts:ungett(sign)
				break
			end
			local rs = acquirenode(ts, next, 'element')
			result = createnode{
				name = 'expr.invoke',
				opname = binaryname[sign:gettype()],
				spos = result.spos,
				epos = rs.epos,
				filename = ts.filename,
				args = {result, rs},
			}
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
			if not set[sign:gettype()] then
				ts:ungett(sign)
				break
			end
			local rs = acquirenode(ts, next, 'element')
			local i = table.append(elist, rs)
			slist[i-1] = sign:gettype()
		end
		local result = elist[#elist]
		for i = #elist-1, 1, -1 do
			local ls = elist[i]
			result = createnode{
				name = 'expr.invoke',
				opname = binaryname[slist[i]],
				spos = ls.spos,
				epos = result.epos,
				filename = ts.filename,
				args = {ls, result},
			}
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

syntax.stat['class'] = function(ts, lead)
	local classname = acquiretoken(ts, 'identifier')
	local parent
	local token = ts:gett()
	if token:gettype() == ':' then
		parent = acquirenode(ts, syntax.expr.main, 'parent')
	else
		ts:ungett(token)
	end
	local body = syntax.block(
		ts, 'class.body', syntax.class.member.main, kwset_end)
	token = ts:gett()
	return createnode{
		name = 'stat.class',
		spos = lead:getspos(),
		epos = token:getepos(),
		filename = ts.filename,
		classname = classname:getcontent(),
		parent = parent,
		body = body,
	}
end

syntax.stat['const'] = function(ts, lead)
	local targetname = acquiretoken(ts, 'identifier')
	local sign = acquiretoken(ts, '=')
	local value = acquirenode(ts, syntax.expr.main, 'expression')
	return createnode{
		name = 'stat.const',
		spos = lead:getspos(),
		epos = value.epos,
		filename = ts.filename,
		targetname = targetname:getcontent(),
		value = value,
	}
end

syntax.stat['function'] = function(ts, lead)
	local targetname = acquiretoken(ts, 'identifier')
	local args = acquirenode(ts, syntax.farglist, 'arglist')
	local rettype
	local token = ts:gett()
	if token:gettype() == ':' then
		rettype = acquirenode(ts, syntax.expr.main, 'return type')
	else
		ts:ungett(token)
	end
	local body = syntax.block(ts, 'block', syntax.stat.main, kwset_end)
	token = ts:gett()
	return createnode{
		name = 'stat.function',
		spos = lead:getspos(),
		epos = token:getepos(),
		filename = ts.filename,
		targetname = targetname:getcontent(),
		args = args,
		rettype = rettype,
		body = body,
	}
end

syntax.stat['operator'] = function(ts, lead)
	local opname = acquiretoken(ts, 'identifier')
	local args = acquirenode(ts, syntax.farglist, 'arglist')
	local rettype
	local token = ts:gett()
	if token:gettype() == ':' then
		rettype = acquirenode(ts, syntax.expr.main, 'return type')
	else
		ts:ungett(token)
	end
	local body = syntax.block(ts, 'block', syntax.stat.main, kwset_end)
	token = ts:gett()
	return createnode{
		name = 'stat.operator',
		spos = lead:getspos(),
		epos = token:getepos(),
		filename = ts.filename,
		opname = opname and opname:getcontent(),
		args = args,
		rettype = rettype,
		body = body,
	}
end

syntax.stat['local'] = function(ts, lead)
	local typev = acquirenode(ts, syntax.expr.main, 'type')
	local targetname = acquiretoken(ts, 'identifier')
	local sign = ts:gett()
	if sign:gettype() ~= '=' then
		ts:ungett(sign)
		return createnode{
			name = 'stat.local',
			spos = lead:getspos(),
			epos = targetname:getepos(),
			filename = ts.filename,
			typev = typev,
			targetname = targetname:getcontent(),
		}
	end
	local value = acquirenode(ts, syntax.expr.main, 'expression')
	return createnode{
		name = 'stat.local',
		spos = lead:getspos(),
		epos = value.epos,
		filename = ts.filename,
		typev = typev,
		targetname = targetname:getcontent(),
		value = value,
	}
end

function syntax.stat.expr(ts)
	local value = syntax.expr.main(ts)
	if not value then
		return
	end
	return createnode{
		name = 'stat.expression',
		spos = value.spos,
		epos = value.epos,
		filename = ts.filename,
		value = value,
	}
end

syntax.stat.main = selector_gen(syntax.stat, syntax.stat.expr)

function syntax.block(ts, nodename, statparser, termtypes)
	local token = ts:peekt()
	local spos = token:getspos()
	local statements = {}
	while true do
		token = ts:peekt()
		local ttype = token:gettype()
		if ttype == 'eof' then
			ts:error('block end expected', token:getspos())
		elseif termtypes[ttype] then
			break
		else
			local stat = acquirenode(ts, statparser, 'statement')
			table.append(statements, stat)
		end
	end
	return createnode{
		name = nodename,
		spos = spos,
		epos = token:getspos(),
		filename = ts.filename,
		statements = statements,
	}
end

syntax.class = {}

syntax.class.member = {}

syntax.class.member['local'] = function(ts, lead)
	local typev = acquirenode(ts, syntax.expr.main, 'type')
	local targetname = acquiretoken(ts, 'identifier')
	return createnode{
		name = 'class.member.local',
		spos = lead:getspos(),
		epos = targetname:getepos(),
		filename = ts.filename,
		typev = typev,
		targetname = targetname:getcontent(),
	}
end

syntax.class.member.main = selector_gen(syntax.class.member)
