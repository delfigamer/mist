local modname = ...
local common

local translate_trace

local function assert_identifier(str)
	return string.match(str, '^[%a_][%w_]*$') or error('invalid bytecode')
end

local function assert_arghead(arg, head)
	if arg[0] ~= head then
		error('invalid bytecode')
	else
		return arg
	end
end

local function ssa_name(id)
	return '_s_' .. common.base26(id)
end

local function local_name(name)
	return '_l_' .. name
end

local translate_value

local translate_arg = {}

translate_arg['function'] = function(arg, tracecontext)
	local argstr = {}
	for i, argname in ipairs(arg[1]) do
		argstr[i] = local_name(assert_identifier(argname))
	end
	argstr = table.concat(argstr, ',')
	local bodystr = translate_trace(arg[2], tracecontext.modulecontext)
	return string.format('function(%s)\n%send', argstr, bodystr)
end

translate_arg['local'] = function(arg, tracecontext)
	return local_name(assert_identifier(arg[1]))
end

translate_arg['member'] = function(arg, tracecontext)
	local basestr = translate_value(arg[1], tracecontext)
	return string.format('%s.%s', basestr, assert_identifier(arg[2]))
end

translate_arg['number'] = function(arg, tracecontext)
	return common.dtos(arg[1])
end

translate_arg['ssa'] = function(arg, tracecontext)
	local id = arg[1]
	if id == 0 then
		return 'nil'
	end
	local name = ssa_name(id)
	if not tracecontext.ssa_used[id] then
		tracecontext.ssa_used[id] = true
		table.append(tracecontext.ssa_lines, name)
	end
	return name
end

translate_arg['string'] = function(arg, tracecontext)
	return string.format('%q', arg[1])
end

function translate_value(arg, tracecontext)
	local afunc = arg and translate_arg[arg[0]]
	if afunc then
		return afunc(arg, tracecontext)
	else
		error('invalid bytecode ' .. tostring(arg))
	end
end

local function translate_vlist(arg, tracecontext)
	assert_arghead(arg, nil)
	local tlist = {}
	for i, item in ipairs(arg) do
		tlist[i] = translate_value(item, tracecontext)
	end
	return tlist
end

local translate_token = {}

local function translate_binary_gen(symbol)
	local format = '%s=%s' .. symbol .. '%s\n'
	return function(token, tracecontext)
		local sa = translate_value(token[1], tracecontext)
		local sb = translate_value(token[2], tracecontext)
		local target = translate_value(token[3], tracecontext)
		return string.format(format, target, sa, sb)
	end
end

local function translate_unary_gen(symbol)
	local format = '%s=' .. symbol .. '%s\n'
	return function(token, tracecontext)
		local sa = translate_value(token[1], tracecontext)
		local target = translate_value(token[2], tracecontext)
		return string.format(format, target, sa)
	end
end

translate_token['add'] = translate_binary_gen('+')

translate_token['ancillary'] = function(token, tracecontext)
	-- do return '' end
	if token[1] then
		local name = tostring(token[1])
		if name == 'position' then
			local row = tonumber(token[2]) or -1
			local col = tonumber(token[3]) or -1
			local filename = tostring(token[4])
			return string.format('--[[ %i:%i ]]\n', row, col)
		end
	else
		return ''
	end
end

translate_token['call'] = function(token, tracecontext)
	local func = translate_value(token[1], tracecontext)
	local args = translate_vlist(token[2], tracecontext)
	local results = translate_vlist(token[3], tracecontext)
	if #results > 0 then
		return string.format('%s=%s(%s)\n',
			table.concat(results, ','), func, table.concat(args, ','))
	else
		return string.format('%s(%s)\n', func, table.concat(args, ','))
	end
end

translate_token['call_method'] = function(token, tracecontext)
	local object= translate_value(token[1], tracecontext)
	local name = assert_arghead(token[2], 'local')[1]
	assert_identifier(name)
	local args = translate_vlist(token[3], tracecontext)
	local results = translate_vlist(token[4], tracecontext)
	if #results > 0 then
		return string.format('%s=%s:%s(%s)\n',
			table.concat(results, ','), object, name, table.concat(args, ','))
	else
		return string.format('%s:%s(%s)\n', object, name, table.concat(args, ','))
	end
end

translate_token['concat'] = translate_binary_gen('..')

translate_token['div'] = translate_binary_gen('/')

translate_token['instantiate'] = function(token, tracecontext)
	local class = translate_value(token[1], tracecontext)
	local target = translate_value(token[2], tracecontext)
	return string.format('%s=%s:new()\n', target, class)
end

translate_token['local_create'] = function(token, tracecontext)
	local target = assert_arghead(token[2], 'local')[1]
	target = local_name(assert_identifier(target))
	local source = translate_value(token[1], tracecontext)
	if source == 'nil' then
		return string.format('local %s\n', target)
	else
		return string.format('local %s=%s\n', target, source)
	end
end

translate_token['move'] = function(token, tracecontext)
	local source = translate_value(token[1], tracecontext)
	local target = translate_value(token[2], tracecontext)
	return string.format('%s=%s\n', target, source)
end

translate_token['mul'] = translate_binary_gen('*')

translate_token['negate'] = translate_unary_gen('-')

translate_token['newclass'] = function(token, tracecontext)
	local parent = translate_value(token[1], tracecontext)
	local name = translate_value(token[2], tracecontext)
	local target = translate_value(token[3], tracecontext)
	if parent == 'nil' then
		tracecontext.modulecontext.base_object_required = true
		parent = '_exl_base_object'
	end
	return string.format('%s=%s:derive(%s)\n', target, parent, name)
end

translate_token['return'] = function(token, tracecontext)
	local values = translate_vlist(token[1], tracecontext)
	return string.format('return %s\n', table.concat(values, ','))
end

translate_token['sub'] = translate_binary_gen('-')

function translate_trace(trace, modulecontext)
	local tracecontext = {
		ssa_used = {},
		ssa_lines = {},
		modulecontext = modulecontext,
	}
	local lines = {}
	for i, token in ipairs(trace) do
		local tfunc = translate_token[token[0]]
		if tfunc then
			lines[i] = tfunc(token, tracecontext)
		else
			error('invalid bytecode')
		end
	end
	local prefix = ''
	if #tracecontext.ssa_lines > 0 then
		prefix = prefix ..
			string.format('local %s\n',
				table.concat(tracecontext.ssa_lines, ','))
	end
	local suffix = ''
	return prefix .. table.concat(lines) .. suffix
end

local function translate_module(trace, it)
	local modulecontext = {
		base_object_required = false,
	}
	local traceres = translate_trace(trace, modulecontext)
	local prefix = 'local _exl_modname = ...\n' ..
		'local _exl_module = package.modtable(_exl_modname)\n'
	if modulecontext.base_object_required then
		prefix = prefix ..
			'local _exl_base_object = require("base.object")\n'
	end
	local suffix = ''
	return prefix .. traceres .. suffix
end

package.modtable(modname, translate_module)

common = require(modname, 2, 'common')
