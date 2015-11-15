local modname = ...
local syntax = package.modtable(modname)
local common = require('exl.common')

local createnode = common.createnode

local function acquiretoken(ts, context, type)
	local token = ts:gett(context)
	if token:gettype() ~= type then
		ts:ungett(token, context)
		context:log('error', type..' expected', token:getspos())
	else
		return token
	end
end

local function acquirenode(ts, context, func, name)
	local node = func(ts, context)
	if not node then
		context:log('error', name..' expected', ts:getpos())
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

syntax.expr.element['number'] = function(ts, context, lead)
	return createnode{
		name = 'expr.number',
		spos = lead:getspos(),
		epos = lead:getepos(),
		value = lead:getcontent(),
	}
end

syntax.expr.element['string'] = function(ts, context, lead)
	return createnode{
		name = 'expr.string',
		spos = lead:getspos(),
		epos = lead:getepos(),
		value = lead:getcontent(),
	}
end

syntax.expr.element['void'] = function(ts, context, lead)
	return createnode{
		name = 'expr.void',
		spos = lead:getspos(),
		epos = lead:getepos(),
	}
end

syntax.expr.element['nil'] = function(ts, context, lead)
	return createnode{
		name = 'expr.nil',
		spos = lead:getspos(),
		epos = lead:getepos(),
	}
end

syntax.expr.element['identifier'] = function(ts, context, lead)
	return createnode{
		name = 'expr.identifier',
		spos = lead:getspos(),
		epos = lead:getepos(),
		targetname = lead:getcontent(),
	}
end

syntax.expr.element['('] = function(ts, context, lead)
	local value = acquirenode(ts, content, syntax.expr.main, 'expression')
	local et = acquiretoken(ts, content, ')')
	return createnode{
		name = 'expr.subexpression',
		spos = lead:getspos(),
		epos = et and et:getepos() or value and value.epos or lead:getepos(),
		value = value,
	}
end

-- function syntax.arglist

syntax.expr.element['function'] = function(ts, context, lead)
	local rettype = acquirenode(ts, context, syntax.expr.main, 'type')
	local body = syntax.body(ts, context, kwset_end)
	return createnode{
		name = 'expr.function',
		spos = lead:getspos(),
		epos = body.epos,
		rettype = rettype,
		body = body,
	}
end

function syntax.expr.element.main(ts, context)
	local lead = ts:gett(context)
	local func = syntax.expr.element[lead:gettype()]
	if func then
		return func(ts, context, lead)
	else
		ts:ungett(lead, context)
		return
	end
end

local binaryname = {
	['+'] = 'add',
	['-'] = 'sub',
	['~'] = 'bxor',
	['=='] = 'eq',
	['~='] = 'neq',
	['+='] = 'adda',
	['-='] = 'suba',
}

local function binary_gen(first, next, ...)
	local set = table.makeset{...}
	return function(ts, context)
		local result = first(ts, context)
		if not result then
			return
		end
		while true do
			local sign = ts:gett(context)
			if not set[sign:gettype()] then
				ts:ungett(sign, context)
				break
			end
			local rs = acquirenode(
				ts, context, next, 'element')
			if not rs then
				break
			end
			result = createnode{
				name = 'expr.binary',
				operator = binaryname[sign:gettype()],
				spos = result.spos,
				epos = rs.epos,
				left = result,
				right = rs,
			}
		end
		return result
	end
end

syntax.expr.sum = binary_gen(
	syntax.expr.element.main, syntax.expr.element.main, '+', '-')

function syntax.expr.main(ts, context)
	return syntax.expr.sum(ts, context)
end

-- statement parsers
-- the name must correspond to the leading keyword, e.g. "const", "function"
-- the stream is right past the leading keyword
-- on success, consumes the definition and returns its AST
-- as the leading keyword is already consumed, these cannot fail legitimately
-- on malformed, consumes up to the error and returns an approximate AST, possibly with some fields missing
syntax.stat = {}

function syntax.stat.const(ts, context, lead)
	local targetname = acquiretoken(ts, context, 'identifier')
	if not targetname then
		return createnode{
			name = 'stat.const',
			spos = lead:getspos(),
			epos = lead:getepos(),
		}
	end
	local sign = acquiretoken(ts, context, '=')
	if not sign then
		return createnode{
			name = 'stat.const',
			spos = lead:getspos(),
			epos = targetname:getepos(),
			targetname = targetname:getcontent(),
		}
	end
	local value = acquirenode(ts, context, syntax.expr.main, 'expression')
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

do
	local set = table.makeset{'=', '+=', '-='}
	function syntax.stat.expr(ts, context)
		local ls = syntax.expr.main(ts, context)
		if not ls then
			return
		end
		local sign = ts:gett(context)
		if not set[sign:gettype()] then
			ts:ungett(sign, context)
			return createnode{
				name = 'stat.expression',
				spos = ls.spos,
				epos = ls.epos,
				value = ls,
			}
		end
		local rs = acquirenode(
			ts, context, syntax.expr.sum, 'expression')
		return createnode{
			name = 'stat.assign',
			operator = binaryname[sign:gettype()],
			spos = ls.spos,
			epos = rs.epos,
			left = ls,
			right = rs,
		}
	end
end

function syntax.stat.main(ts, context)
	local lead = ts:gett(context)
	local func = syntax.stat[lead:gettype()]
	if func then
		return func(ts, context, lead)
	else
		ts:ungett(lead, context)
		return syntax.stat.expr(ts, context)
	end
end

-- body parser
-- consumes a sequence of statements until one of the terminating tokens is reached (e.g. 'end', 'else' or 'until')
-- on success, consumes the statements and the terminating token
-- tokens not recognized as statements are skipped
-- 'eof' is always a terminating token, though encountering it is treated as an error
function syntax.body(ts, context, termtypes)
	local token = ts:peekt(context)
	local spos = token:getspos()
	local statements = {}
	local errorwritten = false
	while true do
		token = ts:peekt(context)
		local ttype = token:gettype()
		if ttype == 'eof' then
			context:log('error', '"end" expected', token:getspos())
			break
		elseif termtypes[ttype] then
			ts:gett(context)
			break
		else
			local stat = syntax.stat.main(ts, context)
			if stat then
				errorwritten = false
				table.append(statements, stat)
			else
				local token = ts:gett(context)
				if not errorwritten then
					context:log('error', 'statement expected', token:getspos())
					errorwritten = true
				end
			end
		end
	end
	return createnode{
		name = 'body',
		spos = spos,
		epos = token:getepos(),
		statements = statements,
	}
end
