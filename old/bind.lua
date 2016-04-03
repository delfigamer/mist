assert(loadfile('base.lua'))()
local parser = require('bind-parser.lua')

-- R_CLASS()
--   name = "..."
--   R_METHOD()
--     name = "..."
--     stringwrap
--     refwrap
--     addref
--     release
--     noluamethod
--   R_STRUCT() ... R_END()
-- R_ENUM()
--   name = "..."
-- R_EMIT() ... R_END()
--   target =
--     hpp_start
--     hpp_beforeincludes
--     hpp_beforeclasses
--     hpp_beforemldecl
--     hpp_mldeclstart
--     hpp_mldeclend
--     hpp_beforemldef
--     hpp_end
--     cpp_start
--     cpp_beforemethods
--     cpp_beforemldef
--     cpp_end
--     ffi_start
--     ffi_beforemldecl
--     ffi_mldeclstart
--     ffi_mldeclend
--     ffi_end
--     lua_start
--     lua_beforeclasses
--     lua_beforemethods
--     lua_beforeenums
--     lua_beforemetatypes
--     lua_end

local extraclasses
local extraheaders
local structname
local fileprefix
local packageprefix
local defaultparent
local compactffi

local filelist = {...}

local function bind(f, ...)
	local argc, argv = select('#', ...), {...}
	for i = 1, argc do
		local inf = f
		local bv = argv[i]
		f = function(...)
			return inf(bv, ...)
		end
	end
	return f
end

local type_map = {}

local function type_lookup(ns, cname)
	if string.sub(cname, 1, 2) == '::' then
		return type_map[cname]
	end
	local currentns = ns
	while currentns do
		local fcname = currentns .. cname
		local typedef = type_map[fcname]
		if typedef then
			return typedef
		end
		currentns = string.match(currentns, '^(.*::).-::')
	end
end

local header_list = {}
local class_list = {}
local method_list = {}
local enum_list = {}
local emit_list = {}

local function parse_metastr(input)
	local result = {}
	for item in string.sub(input, 2, -2):gmatch('([^%s,][^,]*[^%s,])') do
		local name, value = item:match('(.*%S)%s*=%s*(.*%S)')
		if name then
			result[name] = value
		else
			result[item] = true
		end
	end
	return result
end

local function parse_parentstr(input)
	return string.match(input, '^:%s*public%s+(%S*)') or
		string.match(input, '^:%s*private%s+(%S*)') or
		string.match(input, '^:%s*(%S*)')
end

local function parse_rettype(input)
	local rettype = string.match(input, 'static%s+(.*)')
	local bstatic = not not rettype
	rettype = rettype or input
	return bstatic, rettype ~= 'void' and rettype
end

local function parse_attrstr(input)
	local result = {}
	for item in string.gmatch(input, '(%S+)') do
		result[string.lower(item)] = true
	end
	return result
end

local function parse_arglist(input)
	local result = {}
	for item in string.sub(input, 2, -2):gmatch('([^%s,][^,]*[^%s,])') do
		local type, name = item:match('(.-)%s*([%a_][%w_]*)$')
		local arg = {
			type = type,
			name = name,
		}
		table.append(result, arg)
	end
	return result
end

local function parse_method(ns, class, metastr, rettype, cname, arglist, attrs)
	local meta = parse_metastr(metastr)
	local bstatic
	bstatic, rettype = parse_rettype(rettype)
	local method = {
		meta = meta,
		class = class,
		cname = cname,
		lname = meta.name or cname,
		bstatic = bstatic,
		rettype = rettype,
		arglist = parse_arglist(arglist),
		attrs = parse_attrstr(attrs),
		namespace = ns,
	}
	method.fname = class.lname .. '_' .. method.lname
	method.flname = method.fname
	table.append(method_list, method)
	return ''
end

local function parse_structfield(ns, class, fieldstr)
	table.append(class.fields, fieldstr)
end

local function parse_structbody(ns, class, metastr, body)
	body = string.gsub(
		body,
		'/%*.-%*/',
		'')
	body = string.gsub(
		body,
		'//[^\n]*',
		'')
	body = string.gsub(
		body,
		'%s*([^;{]*[^%s;{])%s*;',
		bind(parse_structfield, ns, class))
end

local function parse_class(ns, metastr, keyword, cname, parentstr, body)
	local meta = parse_metastr(metastr)
	local class = {
		meta = meta,
		keyword = keyword,
		cname = cname,
		lname = meta.name or cname,
		fcname = ns .. cname,
		parentname = parse_parentstr(parentstr),
		namespace = ns,
		fields = {},
	}
	class.needsbox = keyword == 'class' or meta.needsbox
	type_map[class.fcname] = class
	table.append(class_list, class)
	body = string.gsub(
		body,
		'R_METHOD(%b())%s*\z
			(.-)%s*\z
			([%a_][%w_]*)%s*\z
			(%b())%s*\z
			([^;{]*)',
		bind(parse_method, ns, class))
	body = string.gsub(
		body,
		'R_STRUCT(%b())\z
			(.-)\z
			R_END%b()',
		bind(parse_structbody, ns, class))
	return ''
end

local function parse_enumfield(ns, enum, name, value)
	local nv = tonumber(value)
	if nv then
		table.append(enum.fields, {
			name = name,
			value = nv,
		})
	end
end

local function parse_enum(ns, metastr, cname, body)
	local meta = parse_metastr(metastr)
	local enum = {
		meta = meta,
		cname = cname,
		lname = meta.name or cname,
		namespace = ns,
		fields = {},
	}
	table.append(enum_list, enum)
	body = string.gsub(
		body,
		'/%*.-%*/',
		'')
	body = string.gsub(
		body,
		'//[^\n]*',
		'')
	body = string.gsub(
		body,
		'%s*([%a_][%w_]*)%s*=%s*([xX%x]+)%s*,',
		bind(parse_enumfield, ns, enum))
	return ''
end

local function parse_emit(metastr, content)
	local meta = parse_metastr(metastr)
	local part = {
		meta = meta,
		content = content,
		target = meta.target or '',
	}
	table.append(emit_list, part)
	return ''
end

local parse_namespace

local function parse_namespace_body(ns, body)
	body = string.gsub(
		body,
		'R_EMIT(%b())\z
			(.-)\z
			R_END%b()',
		parse_emit)
	body = string.gsub(
		body,
		'R_ENUM(%b())%s*\z
			namespace%s+([%a_][%w_]*)%s*{%s*enum%s*\z
			(%b{})%s*;%s*}',
		bind(parse_enum, ns))
	body = string.gsub(
		body,
		'namespace%s+([%a_][%w_]*)%s*\z
			(%b{})',
		bind(parse_namespace, ns))
	body = string.gsub(
		body,
		'R_CLASS(%b())%s*\z
			([%a_][%w_]*)%s+([%a_][%w_]*)%s*([%s%w_,:]*)\z
			(%b{})',
		bind(parse_class, ns))
end

function parse_namespace(ns, name, body)
	parse_namespace_body(ns .. name .. '::', body)
	return ''
end

local function parse_file(source)
	parse_namespace_body('::', source)
end

local function build_type(ns, typestr)
	local base, rest = string.match(typestr, '([%a:_][%w:_]*)(.*)')
	if base then
		local class = type_lookup(ns, base)
		if class then
			if class.needsbox and
				(string.match(rest, '^%s*%*%s*$') or
				string.match(rest, '^%s*const%s*%*%s*$'))
			then
				return class.lname, class.fcname .. rest, class.lname .. rest
			else
				return nil, class.fcname .. rest, class.lname .. rest
			end
		end
	end
	return nil, typestr, typestr
end

local function sort_class_list()
	local result = {}
	local ready = {}
	local current = class_list
	while #current ~= 0 do
		local next = {}
		for i, class in ipairs(current) do
			if not class.parent or ready[class.parent] then
				ready[class] = true
				table.append(result, class)
			else
				table.append(next, class)
			end
		end
		if #next == #current then
			error('cannot resolve class inheritance')
		end
		current = next
	end
	class_list = result
end

local function build_class(class)
	local parentname
	if class.parentname then
		local parent = type_lookup(class.namespace, class.parentname)
		if not parent then
			error(string.format('class %s has unknown parent %s',
				class.fcname, class.parentname))
		end
		class.parent = parent
		class.luadef = 'local ' .. class.lname .. ' = \z
				' .. parent.lname .. ':module(\z
				\'' .. packageprefix .. class.lname .. '\')\n'
	else
		parentname = defaultparent
		class.luadef = 'local ' .. class.lname .. ' = \z
				default_base:module(\z
				\'' .. packageprefix .. class.lname .. '\')\n'
	end
	if not class.needsbox or #class.fields > 0 then
		local fieldstr = {}
		for i, field in ipairs(class.fields) do
			local classbox, ctype, ffitype = build_type(class.namespace, field)
			table.append(fieldstr, '\t' .. ffitype .. ';\n')
		end
		fieldstr = table.concat(fieldstr)
		class.ffidef = 'typedef struct {\n' .. fieldstr .. '}\z
			' .. class.lname .. ';\n'
	else
		class.ffidef = 'typedef void* \z
			' .. class.lname .. ';\n'
	end
	if class.needsbox then
		class.luametabuild = class.lname .. '.typedef = \z
			\'struct{' .. class.lname .. '*ptr;}\'\n\z
			' .. class.lname .. ':buildmetatype()\n'
	else
		class.luametabuild = class.lname .. '.typedef = \z
			\'' .. class.lname .. '\'\n\z
			' .. class.lname .. ':buildmetatype()\n'
	end
end

local function build_method(method)
	local cdefargstr = {}
	local ccallargstr = {}
	local ffidefargstr = {}
	local luadefargstr = {}
	local luacallargstr = {}
	local cfuncstr
	if method.bstatic then
		cfuncstr = method.class.fcname .. '::' .. method.cname
	else
		local cv
		if method.attrs.const then
			cv = ' const'
		else
			cv = ''
		end
		cdefargstr[1] = method.class.fcname .. cv .. '* self'
		ffidefargstr[1] = method.class.lname .. cv .. '* self'
		if method.class.needsbox then
			luacallargstr[1] = 'self.ptr'
		else
			luacallargstr[1] = 'self'
		end
		cfuncstr = 'self->' .. method.cname
	end
	for i, arg in ipairs(method.arglist) do
		local classbox, ctype, ffitype = build_type(method.namespace, arg.type)
		table.append(cdefargstr, ctype .. ' ' .. arg.name)
		table.append(ccallargstr, arg.name)
		table.append(ffidefargstr, ffitype .. ' ' .. arg.name)
		table.append(luadefargstr, arg.name)
		if classbox then
			table.append(luacallargstr, arg.name .. ' and ' .. arg.name .. '.ptr')
		else
			table.append(luacallargstr, arg.name)
		end
	end
	local resultstr
	local luaresultinit
	local luareturnstr
	if method.rettype then
		local classbox, ctype, ffitype =
			build_type(method.namespace, method.rettype)
		table.append(cdefargstr, ctype .. '* result')
		table.append(ffidefargstr, ffitype .. '* result')
		table.append(luacallargstr, 'result')
		resultstr = '*result = '
		luaresultinit = '\tlocal result = ffi.new(\'' .. ffitype .. '[1]\')\n'
		if classbox then
			luareturnstr = ' result[0] ~= nil and ' .. classbox ..
				':new(result[0])'
		else
			if method.meta.stringwrap then
				luareturnstr = ' result[0] ~= nil and ffi.string(result[0])'
			else
				luareturnstr = ' result[0]'
			end
		end
	else
		resultstr = ''
		luaresultinit = ''
		luareturnstr = ''
	end
	cdefargstr = table.concat(cdefargstr, ', ')
	ccallargstr = table.concat(ccallargstr, ', ')
	ffidefargstr = table.concat(ffidefargstr, ', ')
	luadefargstr = table.concat(luadefargstr, ', ')
	luacallargstr = table.concat(luacallargstr, ', ')
	local try_block_c, catch_block_c
	local if_block_lua, then_block_lua, else_block_lua
	if method.attrs.noexcept then
		try_block_c = ''
		catch_block_c = ''
		if_block_lua = ''
		then_block_lua = ''
		else_block_lua = ''
	else
		try_block_c = '\ttry\n'
		catch_block_c = '\tcatch( ::std::exception const& e )\n\z
			\t{\n\z
			\t\t::utils::cbase::seterror(e.what());\n\z
			\t\treturn false;\n\z
			\t}\n\z
			\tcatch(...)\n\z
			\t{\n\z
			\t\t::utils::cbase::seterror("unknown exception");\n\z
			\t\treturn false;\n\z
			\t}\n'
		if_block_lua = 'if '
		then_block_lua = ' then'
		else_block_lua = '\telse\n\z
			\t\terror(cbase:geterror())\n\z
			\tend\n'
	end
	local addrefstr = {}
	if method.meta.addref then
		for name in string.gmatch(method.meta.addref, '(%S+)') do
			table.append(addrefstr, '\t\treference_addref(')
			table.append(addrefstr, name)
			table.append(addrefstr, ')\n')
		end
	end
	addrefstr = table.concat(addrefstr)
	local releasestr
	if method.meta.release then
		releasestr = '\t\tself[0] = nil\n'
	else
		releasestr = ''
	end
	method.cmethod = 'static bool ' .. method.flname .. '\z
			(' .. cdefargstr .. ') NOEXCEPT\n\z
		{\n\z
		' .. try_block_c .. '\z
		\t{\n\z
		\t\t' .. resultstr .. cfuncstr .. '(' .. ccallargstr .. ');\n\z
		\t\treturn true;\n\z
		\t}\n\z
		' .. catch_block_c .. '\z
		}\n\z
		\n'
	method.clistfield = 'bool(*' .. method.flname .. ')\z
		(' .. cdefargstr .. ') NOEXCEPT;'
	method.ffilistfield = 'bool(*' .. method.flname .. ')\z
		(' .. ffidefargstr .. ');'
	if method.meta.noluamethod then
		method.luamethod = ''
	else
		method.luamethod = '\nfunction ' .. method.class.lname .. ':\z
				' .. method.lname .. '\z
				(' .. luadefargstr .. ')\n\z
			' .. luaresultinit .. '\z
			\t' .. if_block_lua .. 'methodlist.' .. method.flname .. '\z
				(' .. luacallargstr .. ')' .. then_block_lua .. '\n\z
			' .. releasestr .. '\z
			' .. addrefstr .. '\z
			\t\treturn' .. luareturnstr .. '\n\z
			' .. else_block_lua .. '\z
			end\n'
	end
end

local function build_enum(enum)
	local fieldstr = {}
	for i, field in ipairs(enum.fields) do
		fieldstr[i] = '\t[\'' .. field.name .. '\'] = \z
			' .. tostring(field.value) .. ',\n'
	end
	fieldstr = table.concat(fieldstr)
	enum.luadef = 'local ' .. enum.lname .. ' = package.modtable(\z
		\'' .. packageprefix .. enum.lname .. '\', \z
		table.makeenum{\n\z
		' .. fieldstr .. '\z
		})\n'
end

local function construct_class_defs()
	local defs = {
		classes = {},
	}
	for i, class in ipairs(class_list) do
		local ldefs = defs
		for nslev in string.gmatch(class.namespace, '[^:]+') do
			if not ldefs[nslev] then
				ldefs[nslev] = {
					name = nslev,
					classes = {},
				}
				table.append(ldefs, ldefs[nslev])
			end
			ldefs = ldefs[nslev]
		end
		table.append(ldefs.classes, class)
	end
	return defs
end

local function build_class_defs_c(defs, lp)
	lp = lp or ''
	local parts = {}
	for i, ldefs in ipairs(defs) do
		parts[i] = lp .. 'namespace ' .. ldefs.name .. '\n\z
			' .. lp .. '{\n\z
			' .. build_class_defs_c(ldefs, lp .. '\t') .. '\n\z
			' .. lp .. '}'
	end
	for i, class in ipairs(defs.classes) do
		table.append(parts,
			lp .. class.keyword .. ' ' .. class.cname .. ';')
	end
	return table.concat(parts, '\n')
end

local function append_build_direct(t, target)
	for i, part in ipairs(emit_list) do
		if part.target == target then
			table.append(t, part.content)
		end
	end
end

local function emit_direct(f, target)
	for i, part in ipairs(emit_list) do
		if part.target == target then
			f:write(part.content)
		end
	end
end

local function emit_hpp()
	local mlhpp = assert(io.open(fileprefix .. '.hpp', 'w'))
	emit_direct(mlhpp, 'hpp_start')
	mlhpp:write('#pragma once\n\n')
	emit_direct(mlhpp, 'hpp_beforeincludes')
	for item in string.gmatch(extraheaders, '(%S+)') do
		mlhpp:write('#include <')
		mlhpp:write(item)
		mlhpp:write('>\n')
	end
	mlhpp:write('\n')
	emit_direct(mlhpp, 'hpp_beforeclasses')
	local defs = construct_class_defs()
	mlhpp:write(build_class_defs_c(defs))
	mlhpp:write('\n')
	emit_direct(mlhpp, 'hpp_beforemldecl')
	mlhpp:write('\nstruct ')
	mlhpp:write(structname)
	mlhpp:write('_t\n{\n')
	emit_direct(mlhpp, 'hpp_mldeclstart')
	for i, method in ipairs(method_list) do
		mlhpp:write('\t')
		mlhpp:write(method.clistfield)
		mlhpp:write('\n')
	end
	emit_direct(mlhpp, 'hpp_mldeclend')
	mlhpp:write('};\n')
	emit_direct(mlhpp, 'hpp_beforemldef')
	mlhpp:write('\nextern ')
	mlhpp:write(structname)
	mlhpp:write('_t const ')
	mlhpp:write(structname)
	mlhpp:write(';\n')
	emit_direct(mlhpp, 'hpp_end')
	mlhpp:close()
end

local function emit_cpp()
	local mlcpp = assert(io.open(fileprefix .. '.cpp', 'w'))
	emit_direct(mlcpp, 'cpp_start')
	mlcpp:write('#include <')
	mlcpp:write(fileprefix)
	mlcpp:write('.hpp>\n')
	for i, header in ipairs(header_list) do
		mlcpp:write(string.format('#include <%s>\n', header))
	end
	mlcpp:write('#include <utils/cbase.hpp>\n#include <exception>\n\n')
	emit_direct(mlcpp, 'cpp_beforemethods')
	for i, method in ipairs(method_list) do
		mlcpp:write(method.cmethod)
	end
	emit_direct(mlcpp, 'cpp_beforemldef')
	mlcpp:write(structname)
	mlcpp:write('_t const ')
	mlcpp:write(structname)
	mlcpp:write(' = {\n')
	for i, method in ipairs(method_list) do
		mlcpp:write('\t')
		mlcpp:write(method.flname)
		mlcpp:write(',\n')
	end
	mlcpp:write('};\n')
	emit_direct(mlcpp, 'cpp_end')
	mlcpp:close()
end

local function build_ffi()
	local parts = {}
	append_build_direct(parts, 'ffi_start')
	for i, class in ipairs(class_list) do
		table.append(parts, class.ffidef)
	end
	append_build_direct(parts, 'ffi_beforemldecl')
	table.append(parts, '\ntypedef struct\n{\n')
	append_build_direct(parts, 'ffi_mldeclstart')
	for i, method in ipairs(method_list) do
		table.append(parts, '\t')
		table.append(parts, method.ffilistfield)
		table.append(parts, '\n')
	end
	append_build_direct(parts, 'ffi_mldeclend')
	table.append(parts, '}\n\t')
	table.append(parts, structname)
	table.append(parts, '_t;\n')
	append_build_direct(parts, 'ffi_end')
	local ffistr = table.concat(parts)
	if compactffi then
		ffistr = string.gsub(ffistr, '%s+', ' ')
		ffistr = string.gsub(ffistr, '%s*([^%w_])%s*', '%1')
		ffistr = string.match(ffistr, '^%s*(.-)%s*$')
	end
	return ffistr
end

local function emit_lua()
	local mllua = assert(io.open(fileprefix .. '.lua', 'w'))
	emit_direct(mllua, 'lua_start')
	mllua:write('local ffi = require(\'ffi\')\nffi.cdef[[')
	mllua:write(build_ffi())
	mllua:write(']]\n')
	emit_direct(mllua, 'lua_beforeclasses')
	mllua:write('local default_base = require(\'')
	mllua:write(defaultparent)
	mllua:write('\')\n')
	for i, class in ipairs(class_list) do
		mllua:write(class.luadef)
	end
	emit_direct(mllua, 'lua_beforemethods')
	for i, method in ipairs(method_list) do
		mllua:write(method.luamethod)
	end
	emit_direct(mllua, 'lua_beforeenums')
	for i, enum in ipairs(enum_list) do
		mllua:write(enum.luadef)
	end
	emit_direct(mllua, 'lua_beforemetatypes')
	for i, class in ipairs(class_list) do
		mllua:write(class.luametabuild)
	end
	emit_direct(mllua, 'lua_end')
	mllua:close()
end

local function main()
	extraclasses = _G.extraclasses or ''
	extraheaders = _G.extraheaders or ''
	structname = _G.structname or error('global "structname" must be set')
	fileprefix = _G.fileprefix or error('global "fileprefix" must be set')
	packageprefix = _G.packageprefix or ''
	defaultparent = _G.defaultparent or
		error('global "defaultparent" must be set')
	compactffi = _G.compactffi
	for i, filename in ipairs(filelist) do
		local modulename = assert(string.match(filename, '(.*)%..*'))
		local source = assert(io.open(filename, 'r')):read('*a')
		table.append(header_list, filename)
		parse_file(source)
	end
	for i, class in ipairs(class_list) do
		build_class(class)
	end
	sort_class_list()
	for i, method in ipairs(method_list) do
		build_method(method)
	end
	for i, enum in ipairs(enum_list) do
		build_enum(enum)
	end
	emit_hpp()
	emit_cpp()
	-- emit_ffi()
	emit_lua()
end

local suc, err = xpcall(main, debug.traceback)
if not suc then
	print(err)
	os.exit(-1)
end
