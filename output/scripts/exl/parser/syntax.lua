local modname = ...
local syntax = package.modtable(modname)
local common = require('exl.common')

local createnode = common.createnode

local function acquiretoken(ts, type)
	local token = ts:gett()
	if token:gettype() ~= type then
		ts:ungett(token)
		ts.env:log('error', type..' expected', token:getspos())
	else
		return token
	end
end

local function acquirenode(ts, func, name)
	local node = func(ts)
	if not node then
		ts.env:log('error', name..' expected', ts:getpos())
	else
		return node
	end
end

local kwset_end = {
	['end'] = true,
}

-- expression parsers
-- tries to parse a corresponding type of expression
-- on success, consumes it and returns its AST
-- on failure, leaves the stream unchanged and returns nil
-- on malformed, consumes up to the error and returns an approximate AST
syntax.expr = {}

-- lowest-level expressions, detected by the type of their lead token
syntax.expr.element = {}

syntax.expr.element['number'] = function(ts, lead)
	return createnode{
		name = 'expr.number',
		spos = lead:getspos(),
		epos = lead:getepos(),
		value = lead:getcontent(),
	}
end

syntax.expr.element['string'] = function(ts, lead)
	return createnode{
		name = 'expr.string',
		spos = lead:getspos(),
		epos = lead:getepos(),
		value = lead:getcontent(),
	}
end

syntax.expr.element['nil'] = function(ts, lead)
	return createnode{
		name = 'expr.nil',
		spos = lead:getspos(),
		epos = lead:getepos(),
	}
end

syntax.expr.element['identifier'] = function(ts, lead)
	return createnode{
		name = 'expr.identifier',
		spos = lead:getspos(),
		epos = lead:getepos(),
		targetname = lead:getcontent(),
	}
end

syntax.expr.element['('] = function(ts, lead)
	local value = acquirenode(ts, syntax.expr.main, 'expression')
	local et = acquiretoken(ts, ')')
	return createnode{
		name = 'expr.subexpression',
		spos = lead:getspos(),
		epos = et and et:getepos() or value and value.epos or lead:getepos(),
		value = value,
	}
end

function syntax.farg(ts)
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
	if not typev then
		node = createnode{
			name = 'expr.function.arg',
			spos = typev.spos,
			epos = typev.epos,
			lvalue = lvalue,
			rvalue = rvalue,
		}
		goto fail
	end
	target = ts:gett()
	if target:gettype() == ',' or target:gettype() == ')' then
		ts:ungett(target)
		return createnode{
			name = 'expr.function.arg',
			spos = typev.spos,
			epos = typev.epos,
			lvalue = lvalue,
			rvalue = rvalue,
			typev = typev,
		}
	elseif target:gettype() == 'identifier' then
		node = createnode{
			name = 'expr.function.arg',
			spos = typev.spos,
			epos = target:getepos(),
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
			ts.env:log('error', ') expected', nt:getspos())
			goto fail
		end
	else
		ts.env:log('error', 'identifier expected', target:getspos())
		node = createnode{
			name = 'expr.function.arg',
			spos = typev.spos,
			epos = target:getepos(),
			lvalue = lvalue,
			rvalue = rvalue,
			typev = typev,
		}
		goto fail
	end
::fail::
	local token
	repeat
		token = ts:gett()
		local tt = token:gettype()
	until tt == ',' or tt == ')' or tt == 'eof'
	ts:ungett(token)
	return node
end

function syntax.farglist(ts)
	local lead = acquiretoken(ts, '(')
	if not lead then
		return createnode{
			name = 'expr.function.arglist',
			spos = ts:getpos(),
			epos = ts:getpos(),
			args = {},
		}
	end
	local nt = ts:peekt()
	if nt:gettype() == ')' then
		ts:gett()
		return createnode{
			name = 'expr.function.arglist',
			spos = lead:getspos(),
			epos = nt:getepos(),
			args = {},
		}
	end
	local args = {}
	while true do
		local arg = syntax.farg(ts)
		table.append(args, arg)
		nt = ts:gett()
		if nt:gettype() == ')' then
			break
		elseif nt:gettype() ~= ',' then
			ts.env:log('error', ') expected', nt:getspos())
		end
	end
	return createnode{
		name = 'expr.function.arglist',
		spos = lead:getspos(),
		epos = nt:getepos(),
		args = args,
	}
end

syntax.expr.element['function'] = function(ts, lead)
	local arglist = acquirenode(ts, syntax.farglist, 'arglist')
	local rettype
	local token = ts:gett()
	if token:gettype() == ':' then
		rettype = acquirenode(ts, syntax.expr.main, 'return type')
	else
		ts:ungett(token)
	end
	local body = syntax.block(ts, kwset_end)
	token = ts:gett()
	return createnode{
		name = 'expr.function',
		spos = lead:getspos(),
		epos = token:getepos(),
		arglist = arglist,
		rettype = rettype,
		body = body,
	}
end

syntax.expr.element['operator'] = function(ts, lead)
	local opname = acquiretoken(ts, 'identifier')
	if not opname then
		return createnode{
			name = 'expr.operator',
			spos = lead:getspos(),
			epos = lead:getepos(),
			args = {},
		}
	end
	local token = ts:gett()
	if token:gettype() ~= '(' or token:islinestart() then
		ts:ungett(token)
		return createnode{
			name = 'expr.operator',
			operator = opname:getcontent(),
			spos = lead:getspos(),
			epos = opname:getepos(),
			args = {},
		}
	end
	token = ts:peekt()
	if token:gettype() == ')' then
		ts:gett()
		return createnode{
			name = 'expr.operator',
			operator = opname:getcontent(),
			spos = lead:getspos(),
			epos = token:getepos(),
			args = {},
		}
	end
	local args = {}
	while true do
		local arg = acquirenode(ts, syntax.expr.main, 'argument')
		if arg then
			table.append(args, arg)
		end
		token = ts:gett()
		if token:gettype() == ')' then
			break
		elseif token:gettype() ~= ',' then
			ts.env:log('error', ') expected', token:getspos())
			repeat
				token = ts:gett()
				local tt = token:gettype()
			until tt == ',' or tt == ')' or tt == 'eof'
		end
	end
	return createnode{
		name = 'expr.operator',
		operator = opname:getcontent(),
		spos = lead:getspos(),
		epos = token:getepos(),
		args = args,
	}
end

syntax.expr.type = {}

syntax.expr.type['function'] = function(ts, lead, nlead)
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
			arglist = arglist,
			rettype = rettype,
		}
	else
		return createnode{
			name = 'expr.function.typev',
			spos = lead:getspos(),
			epos = arglist.epos,
			arglist = arglist,
		}
	end
end

syntax.expr.element['type'] = function(ts, lead)
	local nlead = ts:gett()
	local func = syntax.expr.type[nlead:gettype()]
	if func then
		return func(ts, lead, nlead)
	else
		ts:ungett(nlead)
		return
	end
end

function syntax.expr.element.main(ts)
	local lead = ts:gett()
	local func = syntax.expr.element[lead:gettype()]
	if func then
		return func(ts, lead)
	else
		ts:ungett(lead)
		return
	end
end

syntax.expr.call = function(ts)
	local args = {syntax.expr.element.main(ts)}
	if not args[1] then
		return
	end
::start::
	local token = ts:gett()
	if token:gettype() ~= '(' or token:islinestart() then
		ts:ungett(token)
		return args[1]
	end
	token = ts:peekt()
	if token:gettype() == ')' then
		ts:gett()
		args = {
			createnode{
				name = 'expr.operator',
				operator = 'call',
				spos = args[1].spos,
				epos = token:getepos(),
				args = args,
			},
		}
		goto start
	end
	while true do
		local arg = acquirenode(ts, syntax.expr.main, 'argument')
		if arg then
			table.append(args, arg)
		end
		token = ts:gett()
		if token:gettype() == ')' then
			break
		elseif token:gettype() ~= ',' then
			ts.env:log('error', ') expected', token:getspos())
			repeat
				token = ts:gett()
				local tt = token:gettype()
			until tt == ',' or tt == ')' or tt == 'eof'
		end
	end
	args = {
		createnode{
			name = 'expr.operator',
			operator = 'call',
			spos = args[1].spos,
			epos = token:getepos(),
			args = args,
		},
	}
	goto start
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
			local rs = acquirenode(
				ts, next, 'element')
			if not rs then
				break
			end
			result = createnode{
				name = 'expr.operator',
				operator = binaryname[sign:gettype()],
				spos = result.spos,
				epos = rs.epos,
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
			local rs = acquirenode(
				ts, next, 'element')
			if not rs then
				break
			end
			local i = table.append(elist, rs)
			slist[i-1] = sign:gettype()
		end
		local result = elist[#elist]
		for i = #elist-1, 1, -1 do
			local ls = elist[i]
			result = createnode{
				name = 'expr.operator',
				operator = binaryname[slist[i]],
				spos = ls.spos,
				epos = result.epos,
				args = {ls, result},
			}
		end
		return result
	end
end

syntax.expr.concat = binary_gen(
	syntax.expr.call, syntax.expr.call, '..')
syntax.expr.prod = binary_gen(
	syntax.expr.concat, syntax.expr.concat, '*', '/')
syntax.expr.sum = binary_gen(
	syntax.expr.prod, syntax.expr.prod, '+', '-')
syntax.expr.assign = rbinary_gen(
	syntax.expr.sum, syntax.expr.sum, '=', '+=', '-=')

function syntax.expr.main(ts)
	return syntax.expr.assign(ts)
end

-- statement parsers
-- the name must correspond to the leading keyword, e.g. "const", "function"
-- the stream is right past the leading keyword
-- on success, consumes the definition and returns its AST
-- as the leading keyword is already consumed, these cannot fail legitimately
-- on malformed, consumes up to the error and returns an approximate AST, possibly with some fields missing
syntax.stat = {}

function syntax.stat.const(ts, lead)
	local targetname = acquiretoken(ts, 'identifier')
	if not targetname then
		return createnode{
			name = 'stat.const',
			spos = lead:getspos(),
			epos = lead:getepos(),
		}
	end
	local sign = acquiretoken(ts, '=')
	if not sign then
		return createnode{
			name = 'stat.const',
			spos = lead:getspos(),
			epos = targetname:getepos(),
			targetname = targetname:getcontent(),
		}
	end
	local value = acquirenode(ts, syntax.expr.main, 'expression')
	if not value then
		return createnode{
			name = 'stat.const',
			spos = lead:getspos(),
			epos = sign:getepos(),
			targetname = targetname:getcontent(),
		}
	end
	return createnode{
		name = 'stat.const',
		spos = lead:getspos(),
		epos = value.epos,
		targetname = targetname:getcontent(),
		value = value,
	}
end

syntax.stat['function'] = function(ts, lead)
	local targetname = acquiretoken(ts, 'identifier')
	local arglist = acquirenode(ts, syntax.farglist, 'arglist')
	local rettype
	local token = ts:gett()
	if token:gettype() == ':' then
		rettype = acquirenode(ts, syntax.expr.main, 'return type')
	else
		ts:ungett(token)
	end
	local body = syntax.block(ts, kwset_end)
	token = ts:gett()
	return createnode{
		name = 'stat.function',
		spos = lead:getspos(),
		epos = token:getepos(),
		targetname = targetname:getcontent(),
		arglist = arglist,
		rettype = rettype,
		body = body,
	}
end

syntax.stat['operator'] = function(ts, lead)
	local opname = acquiretoken(ts, 'identifier')
	local arglist = acquirenode(ts, syntax.farglist, 'arglist')
	local rettype
	local token = ts:gett()
	if token:gettype() == ':' then
		rettype = acquirenode(ts, syntax.expr.main, 'return type')
	else
		ts:ungett(token)
	end
	local body = syntax.block(ts, kwset_end)
	token = ts:gett()
	return createnode{
		name = 'stat.operator',
		spos = lead:getspos(),
		epos = token:getepos(),
		operator = opname:getcontent(),
		arglist = arglist,
		rettype = rettype,
		body = body,
	}
end

syntax.stat['local'] = function(ts, lead)
	local typev = acquirenode(ts, syntax.expr.main, 'type')
	if not typev then
		return createnode{
			name = 'stat.local',
			spos = lead:getspos(),
			epos = lead:getepos(),
		}
	end
	local targetname = acquiretoken(ts, 'identifier')
	if not targetname then
		return createnode{
			name = 'stat.local',
			spos = lead:getspos(),
			epos = typev.epos,
			typev = typev,
		}
	end
	local sign = ts:gett()
	if sign:gettype() ~= '=' then
		ts:ungett(sign)
		return createnode{
			name = 'stat.local',
			spos = lead:getspos(),
			epos = targetname:getepos(),
			typev = typev,
			targetname = targetname:getcontent(),
		}
	end
	local value = acquirenode(ts, syntax.expr.main, 'expression')
	if not value then
		return createnode{
			name = 'stat.local',
			spos = lead:getspos(),
			epos = sign:getepos(),
			typev = typev,
			targetname = targetname:getcontent(),
		}
	end
	return createnode{
		name = 'stat.local',
		spos = lead:getspos(),
		epos = value.epos,
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
		value = value,
	}
end

function syntax.stat.main(ts)
	local lead = ts:gett()
	local func = syntax.stat[lead:gettype()]
	if func then
		return func(ts, lead)
	else
		ts:ungett(lead)
		return syntax.stat.expr(ts)
	end
end

-- block parser
-- consumes a sequence of statements until one of the terminating tokens is reached (e.g. 'end', 'else' or 'until')
-- on success, consumes the statements
-- tokens not recognized as statements are skipped
-- 'eof' is always a terminating token, though encountering it is treated as an error
function syntax.block(ts, termtypes)
	local token = ts:peekt()
	local spos = token:getspos()
	local statements = {}
	local errorwritten = false
	while true do
		token = ts:peekt()
		local ttype = token:gettype()
		if ttype == 'eof' then
			ts.env:log('error', 'block end expected', token:getspos())
			break
		elseif termtypes[ttype] then
			break
		else
			local stat = syntax.stat.main(ts)
			if stat then
				errorwritten = false
				table.append(statements, stat)
			else
				local token = ts:gett()
				if not errorwritten then
					ts.env:log('error', 'statement expected', token:getspos())
					errorwritten = true
				end
			end
		end
	end
	return createnode{
		name = 'block',
		spos = spos,
		epos = token:getspos(),
		statements = statements,
	}
end
