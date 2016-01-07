local modname = ...
local common

local translate

local function check_identifier(str)
	return string.match(str, '^[%a_][%w_]*$') or error('invalid bytecode')
end

local function arg_checktype(arg, type)
	if arg.type ~= type then
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

translate_arg['function'] = function(arg, context)
	local argstr = {}
	for i, argname in ipairs(arg.args) do
		argstr[i] = local_name(check_identifier(argname))
	end
	argstr = table.concat(argstr, ',')
	local bodystr = translate(arg.body)
	return string.format('function(%s)\n%send', argstr, bodystr)
end

translate_arg['int'] = function(arg, context)
	return string.format('%i', arg.value)
end

translate_arg['local'] = function(arg, context)
	return local_name(check_identifier(arg.value))
end

translate_arg['member'] = function(arg, context)
	local basestr = translate_value(arg.base, context)
	return string.format('%s.%s', basestr, check_identifier(arg.index))
end

translate_arg['number'] = function(arg, context)
	return common.dtos(arg.value)
end

translate_arg['ssa'] = function(arg, context)
	local id = arg.value
	if id == 0 then
		return 'nil'
	end
	local name = ssa_name(id)
	if not context.ssa_used[id] then
		context.ssa_used[id] = true
		table.append(context.ssa_lines, name)
	end
	return name
end

translate_arg['string'] = function(arg, context)
	return string.format('%q', arg.value)
end

function translate_value(arg, context)
	local afunc = arg and translate_arg[arg.type]
	if afunc then
		return afunc(arg, context)
	else
		error('invalid bytecode ' .. tostring(arg))
	end
end

function translate_vlist(arg, context)
	arg_checktype(arg, 'list')
	local tlist = {}
	for i, item in ipairs(arg.items) do
		tlist[i] = translate_value(item, context)
	end
	return tlist
end

local translate_token = {}

local function translate_binary_gen(symbol)
	local format = '%s=%s' .. symbol .. '%s\n'
	return function(token, context)
		local sa = translate_value(token.args[1], context)
		local sb = translate_value(token.args[2], context)
		local target = translate_value(token.args[3], context)
		return string.format(format, target, sa, sb)
	end
end

local function translate_unary_gen(symbol)
	local format = '%s=' .. symbol .. '%s\n'
	return function(token, context)
		local sa = translate_value(token.args[1], context)
		local target = translate_value(token.args[2], context)
		return string.format(format, target, sa)
	end
end

translate_token['add'] = translate_binary_gen('+')

translate_token['ancillary'] = function(token, context)
	do return '' end
	if token.args[1] then
		local name = arg_checktype(token.args[1], 'string').value
		if name == 'comment' then
			local row = arg_checktype(token.args[2], 'int').value
			local col = arg_checktype(token.args[3], 'int').value
			local filename = arg_checktype(token.args[4], 'string').value
			local text = arg_checktype(token.args[5], 'string').value
			return string.format('--[[%s:%i:%i %s]]\n', filename, row, col, text)
		end
	else
		return ''
	end
end

translate_token['call'] = function(token, context)
	local func = translate_value(token.args[1], context)
	local args = translate_vlist(token.args[2], context)
	local results = translate_vlist(token.args[3], context)
	if results[1] then
		return string.format('%s=%s(%s)\n',
			table.concat(results, ','), func, table.concat(args, ','))
	else
		return string.format('%s(%s)\n', func, table.concat(args, ','))
	end
end

translate_token['call_method'] = function(token, context)
	local object= translate_value(token.args[1], context)
	local name = arg_checktype(token.args[2], 'local').value
	check_identifier(name)
	local args = translate_vlist(token.args[3], context)
	local results = translate_vlist(token.args[4], context)
	if results[1] then
		return string.format('%s=%s:%s(%s)\n',
			table.concat(results, ','), object, name, table.concat(args, ','))
	else
		return string.format('%s:%s(%s)\n', object, name, table.concat(args, ','))
	end
end

translate_token['concat'] = translate_binary_gen('..')

translate_token['div'] = translate_binary_gen('/')

translate_token['local_create'] = function(token, context)
	local target = arg_checktype(token.args[2], 'local').value
	target = local_name(check_identifier(target))
	local source = translate_value(token.args[1], context)
	if source == 'nil' then
		return string.format('local %s\n', target)
	else
		return string.format('local %s=%s\n', target, source)
	end
end

translate_token['move'] = function(token, context)
	local source = translate_value(token.args[1], context)
	local target = translate_value(token.args[2], context)
	return string.format('%s=%s\n', target, source)
end

translate_token['mul'] = translate_binary_gen('*')

translate_token['negate'] = translate_unary_gen('-')

translate_token['newclass'] = function(token, context)
	local parent = translate_value(token.args[1], context)
	local name = translate_value(token.args[2], context)
	local target = translate_value(token.args[3], context)
	if parent == 'nil' then
		parent = 'require("base.object")'
	end
	return string.format('%s=%s:derive(%s)\n', target, parent, name)
end

translate_token['return'] = function(token, context)
	local values = translate_vlist(token.args[1], context)
	return string.format('return %s\n', table.concat(values, ','))
end

translate_token['sub'] = translate_binary_gen('-')

function translate(block)
	local context = {
		ssa_used = {},
		ssa_lines = {},
	}
	local lines = {}
	for i, token in ipairs(block.parts) do
		local tfunc = translate_token[token.opinfo.name]
		if tfunc then
			lines[i] = tfunc(token, context)
		else
			lines[i] = '\t' .. tostring(token) .. '\n'
		end
	end
	local ssadef
	if context.ssa_lines[1] then
		ssadef = string.format('local %s\n', table.concat(context.ssa_lines, ','))
	else
		ssadef = ''
	end
	return ssadef .. table.concat(lines)
end

package.modtable(modname, translate)

common = package.relrequire(modname, 2, 'common')
