local modname = ...
local syntax = package.modtable(modname)
local common = require('exl.common')

local createnode = common.createnode

local function acquiretoken(ts, type)
	local token = ts:gett()
	if token:gettype() ~= type then
		ts:ungett(token)
		ts:getenv():log('error', type..' expected', token:getspos())
	else
		return token
	end
end

local function acquirenode(ts, func, name)
	local node = func(ts)
	if not node then
		ts:getenv():log('error', name..' expected', ts:getpos())
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

syntax.expr.element['void'] = function(ts, lead)
	return createnode{
		name = 'expr.void',
		spos = lead:getspos(),
		epos = lead:getepos(),
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

-- function syntax.arglist

syntax.expr.element['function'] = function(ts, lead)
	local rettype = acquirenode(ts, syntax.expr.main, 'type')
	local body = syntax.block(ts, kwset_end)
	return createnode{
		name = 'expr.function',
		spos = lead:getspos(),
		epos = body.epos,
		rettype = rettype,
		body = body,
	}
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

function syntax.expr.main(ts)
	return syntax.expr.sum(ts)
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

do
	local set = table.makeset{'=', '+=', '-='}
	function syntax.stat.expr(ts)
		local ls = syntax.expr.main(ts)
		if not ls then
			return
		end
		local sign = ts:gett()
		if not set[sign:gettype()] then
			ts:ungett(sign)
			return createnode{
				name = 'stat.expression',
				spos = ls.spos,
				epos = ls.epos,
				value = ls,
			}
		end
		local rs = acquirenode(
			ts, syntax.expr.sum, 'expression')
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
-- on success, consumes the statements and the terminating token
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
			ts:getenv():log('error', 'block end expected', token:getspos())
			break
		elseif termtypes[ttype] then
			ts:gett()
			break
		else
			local stat = syntax.stat.main(ts)
			if stat then
				errorwritten = false
				table.append(statements, stat)
			else
				local token = ts:gett()
				if not errorwritten then
					ts:getenv():log('error', 'statement expected', token:getspos())
					errorwritten = true
				end
			end
		end
	end
	return createnode{
		name = 'block',
		spos = spos,
		epos = token:getepos(),
		statements = statements,
	}
end
