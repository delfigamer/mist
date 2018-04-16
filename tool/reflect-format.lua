local modname = ...
local format = package.modtable(modname)
local ffi = require('ffi')
local env = require('env')
local parser = require('reflect-parser')

local function echo(...)
	local arg = table.pack(...)
	for i = 1, arg.count do
		if arg[i] ~= nil then
			coroutine.yield(tostring(arg[i]))
		end
	end
end

local function q(str)
	return string.format('%q', str)
end

local function sep_next(prev, sep)
	if prev then
		echo(sep)
	end
	return true
end

function format.formatgeneric(writer, func, ...)
	local thread = coroutine.create(
		function(...)
			local ret, err = pcall(func, ...)
			if err then
				error(err)
			end
		end)
	local suc, ret = coroutine.resume(thread, ...)
	while true do
		if not suc then
			error(ret)
		elseif ret then
			writer(ret)
		end
		if coroutine.status(thread) == 'suspended' then
			suc, ret = coroutine.resume(thread)
		else
			break
		end
	end
end

function format.tostring(func, ...)
	local content = {}
	local function writer(part)
		table.append(content, part)
	end
	format.formatgeneric(writer, func, ...)
	return table.concat(content)
end

function format.tofile(path, func, ...)
	env.makepath(path)
	local file = assert(io.open(env.path(path), 'w'))
	local function writer(part)
		file:write(part)
	end
	format.formatgeneric(writer, func, ...)
	file:close()
end

function format.tofileandstring(path, func, ...)
	local content = {}
	env.makepath(path)
	local file = assert(io.open(env.path(path), 'w'))
	local function writer(part)
		file:write(part)
		table.append(content, part)
	end
	format.formatgeneric(writer, func, ...)
	file:close()
	return table.concat(content)
end

local function decldefstring(decl)
	return parser.locationstring(decl.location) .. '\n\t' ..
		parser.decldefstring(decl) .. '\n'
end

local function declaratorstring(decltype, name)
	local prefix = ''
	local suffix = ''
	local center = name or ''
	local needsbrackets = false

	local function addprefix(part)
		needsbrackets = true
		prefix = part .. prefix
	end

	local function addsuffix(part)
		if needsbrackets then
			needsbrackets = false
			center = '(' .. prefix .. ' ' .. center .. suffix .. ')'
			prefix = ''
			suffix = part
		else
			suffix = part .. suffix
		end
	end

	local function cvstr()
		local part = ''
		if decltype.isconst then
			part = part .. ' const'
		end
		return part
	end

	local function combine(basestr)
		return basestr .. prefix .. ' ' .. center .. suffix
	end

	while true do
		if decltype.name then
			return combine(decltype.name .. cvstr())
		elseif decltype.pointer then
			addprefix('*' .. cvstr())
			decltype = decltype.pointer
		elseif decltype.array then
			addsuffix('[' .. tostring(decltype.size) .. ']')
			decltype = decltype.array
		else
			error('unknown type category')
		end
	end
end

function format.cpp_namespace_rfield(env, moduledef, decl)
	echo('\textern "C" ', declaratorstring(decl.interntype:sourcerettype()),
		'r_', decl.commonname, '_accessor(')
	if decl.outerdecl and not decl.isstatic then
		echo(decl.outerdecl.sourcename, '* self')
	end
	echo(')\n')
	echo('\t{\n')
	echo('\t\treturn ', decl.interntype:sourceretprefix())
	if decl.outerdecl and not decl.isstatic then
		echo('self->', decl.sourcelocalname)
	else
		echo(decl.sourcename)
	end
	echo(decl.interntype:sourceretsuffix(), ';\n')
	echo('\t}\n')
	if decl.interntype:isimmutable() then
		return
	end
	echo('\textern "C" void r_', decl.commonname, '_mutator(')
	if decl.outerdecl and not decl.isstatic then
		echo(decl.outerdecl.sourcename, '* self, ')
	end
	echo(declaratorstring(decl.interntype:sourceargtype(), 'value'), ')\n')
	echo('\t{\n')
	echo('\t\t')
	if decl.outerdecl and not decl.isstatic then
		echo('self->', decl.sourcelocalname)
	else
		echo(decl.sourcename)
	end
	echo(' = ', decl.interntype:sourceargprefix(), 'value',
		decl.interntype:sourceargsuffix(), ';\n')
	echo('\t}\n')
end

function format.cpp_namespace_rmethod(env, moduledef, decl)
	echo('\textern "C" ', declaratorstring(decl.rettype:sourcerettype()),
		'r_', decl.commonname, '_wrapper(')
	local sep
	if decl.outerdecl and not decl.isstatic then
		sep = sep_next(sep, ', ')
		echo(decl.outerdecl.sourcename, '* self')
	end
	for i, arg in ipairs(decl.argtypes) do
		sep = sep_next(sep, ', ')
		echo(declaratorstring(arg:sourceargtype(), string.format('arg%i', i)))
	end
	if not decl.isnoexcept then
		sep = sep_next(sep, ', ')
		echo('r::exceptionbox* exception')
	end
	echo(')\n')
	echo('\t{\n')
	if not decl.isnoexcept then
		echo('\t\ttry\n')
		echo('\t\t{\n')
	end
	echo('\t\t\t')
	if not decl.rettype.isvoid then
		echo('return ', decl.rettype:sourceretprefix())
	end
	if decl.outerdecl and not decl.isstatic then
		echo('self->', decl.sourcelocalname)
	else
		echo(decl.sourcename)
	end
	echo('(')
	local sep
	for i, arg in ipairs(decl.argtypes) do
		sep = sep_next(sep, ', ')
		echo(arg:sourceargprefix(), string.format('arg%i', i),
			arg:sourceargsuffix())
	end
	echo(')')
	if not decl.rettype.isvoid then
		echo(decl.rettype:sourceretsuffix())
	end
	echo(';\n')
	if not decl.isnoexcept then
		echo('\t\t}\n')
		echo('\t\tcatch(std::exception const& e)\n')
		echo('\t\t{\n')
		echo('\t\t\tr::exceptionbox::make(exception);\n')
		if not decl.rettype.isvoid then
			echo('\t\t\treturn ', decl.rettype:defaultvalue(), ';\n')
		end
		echo('\t\t}\n')
	end
	echo('\t}\n')
end

function format.cpp_ptrtype_rfield(env, moduledef, decl)
	echo('\t\tdecltype(&r_', decl.commonname, '_accessor) ',
		decl.commonname, '_accessor', ';\n')
	if not decl.interntype:isimmutable() then
		echo('\t\tdecltype(&r_', decl.commonname, '_mutator) ',
			decl.commonname, '_mutator', ';\n')
	end
end

function format.cpp_ptrtype_rmethod(env, moduledef, decl)
	echo('\t\tdecltype(&r_', decl.commonname, '_wrapper) ',
		decl.commonname, ';\n')
end

function format.cpp_ptrinit_rfield(env, moduledef, decl)
	echo('\t\t&r_', decl.commonname, '_accessor,\n')
	if not decl.interntype:isimmutable() then
		echo('\t\t&r_', decl.commonname, '_mutator,\n')
	end
end

function format.cpp_ptrinit_rmethod(env, moduledef, decl)
	echo('\t\t&r_', decl.commonname, '_wrapper,\n')
end

function format.cpp_chunkbytes(env, chunkbytes)
	local linelen = 8
	local ptr = ffi.cast('uint8_t const*', chunkbytes)
	local pend = ptr + #chunkbytes
	while ptr ~= pend do
		if linelen >= 8 then
			echo('\n\t\t')
			linelen = 0
		else
			echo(' ')
		end
		echo(string.format('0x%.2x,', ptr[0]))
		linelen = linelen + 1
		ptr = ptr + 1
	end
end

function format.cpp(env, moduledef)
	echo('#include <', moduledef.source, '>\n')
	echo('#include <reflection.hpp>\n')
	echo('#include <common.hpp>\n')
	echo('namespace\n')
	echo('{\n')
	for i, decl in ipairs(moduledef.decls) do
		echo('/*', decldefstring(decl), '*/\n')
		if decl.rfield then
			format.cpp_namespace_rfield(env, moduledef, decl)
		elseif decl.rmethod then
			format.cpp_namespace_rmethod(env, moduledef, decl)
		end
	end
	echo('\tstruct r_ptr_t\n')
	echo('\t{\n')
	for i, decl in ipairs(moduledef.decls) do
		echo('/*', decldefstring(decl), '*/\n')
		if decl.rfield then
			format.cpp_ptrtype_rfield(env, moduledef, decl)
		elseif decl.rmethod then
			format.cpp_ptrtype_rmethod(env, moduledef, decl)
		end
	end
	echo('\t};\n')
	echo('\tr_ptr_t r_ptr = {\n')
	for i, decl in ipairs(moduledef.decls) do
		echo('/*', decldefstring(decl), '*/\n')
		if decl.rfield then
			format.cpp_ptrinit_rfield(env, moduledef, decl)
		elseif decl.rmethod then
			format.cpp_ptrinit_rmethod(env, moduledef, decl)
		end
	end
	echo('\t};\n')
	echo('\tuint8_t const r_chunk[] = {')
	format.cpp_chunkbytes(env, moduledef.chunkbytes)
	echo('\t};\n')
	echo('}\n')
	echo('extern r::module const r_', moduledef.identname, '_module;\n')
	echo('r::module const r_', moduledef.identname, '_module = {\n')
	echo('\t&r_ptr,\n')
	echo('\t&r_chunk,\n')
	echo('\tsizeof( r_chunk ),\n')
	echo('\t', q(moduledef.name .. '.r.lua'), ' };\n')
end

function format.lua_ffi_rstruct(env, moduledef, decl)
	echo('\ttypedef struct ', decl.commonname, ' {\n')
	for i, field in ipairs(decl.fields) do
		local fieldtype = field.interntype:commonfieldtype()
		if not fieldtype then
			parser.locationerror(field.location,
				'invalid field type')
		end
		echo('\t\t', declaratorstring(fieldtype, field.name[1]), ';\n')
	end
	echo('\t} ', decl.commonname, ';\n')
end

function format.lua_ffi_rclass(env, moduledef, decl)
	echo('\ttypedef void ', decl.commonname, ';\n')
	echo('\ttypedef struct r_', decl.commonname, '_box {\n')
	echo('\t\t', decl.commonname, '* object;\n')
	echo('\t\tvoid(*cfreeptr)(struct r_', decl.commonname, '_box* self);\n')
	echo('\t\tvoid* freeptr;\n')
	echo('\t} r_', decl.commonname, '_box;\n')
end

function format.lua_ffi_rexternal(env, moduledef, decl)
	echo('\ttypedef void ', decl.commonname, ';\n')
	echo('\ttypedef struct r_', decl.commonname, '_box r_',
		decl.commonname, '_box;\n')
end

function format.lua_ptrtype_rfield(env, moduledef, decl)
	echo('\t\t', declaratorstring(decl.interntype:commonrettype()),
		'(*', decl.commonname, '_accessor)(')
	if decl.outerdecl and not decl.isstatic then
		echo(decl.outerdecl.commonname, '* self')
	end
	echo(');\n')
	if decl.interntype:isimmutable() then
		return
	end
	echo('\t\tvoid(*', decl.commonname, '_mutator)(')
	if decl.outerdecl and not decl.isstatic then
		echo(decl.outerdecl.commonname, '* self, ')
	end
	echo(declaratorstring(decl.interntype:commonargtype(), 'value'))
	echo(');\n')
end

function format.lua_ptrtype_rmethod(env, moduledef, decl)
	echo('\t\t', declaratorstring(decl.rettype:commonrettype()),
		'(*', decl.commonname, ')(')
	local sep
	if decl.outerdecl and not decl.isstatic then
		sep = sep_next(sep, ', ')
		echo(decl.outerdecl.commonname, '* self')
	end
	for i, argtype in ipairs(decl.argtypes) do
		sep = sep_next(sep, ', ')
		echo(declaratorstring(argtype:commonargtype(), string.format('arg%i', i)))
	end
	if not decl.isnoexcept then
		sep = sep_next(sep, ', ')
		echo('reflection_exceptionbox* exception')
	end
	echo(');\n')
end

function format.lua_namespace_rstruct(env, moduledef, decl)
	if decl.attrs['r::hidden'] ~= nil then
		return
	end
	echo('local ', decl.commonname,
		' = package.loaded["base.ffipure"]:module(', q(decl.luaname), ')\n')
	echo(decl.commonname,
		'.typedef = ', q(decl.commonname), '\n')
end

function format.lua_namespace_rclass(env, moduledef, decl)
	if decl.attrs['r::hidden'] ~= nil then
		return
	end
	echo('local ', decl.commonname, ' = package.loaded[')
	if decl.base then
		echo(q(decl.base.luaname))
	else
		echo('"reflection.objectbox"')
	end
	echo(']:module(', q(decl.luaname), ')\n')
	echo(decl.commonname,
		'.typedef = ', q('r_' .. decl.commonname .. '_box'), '\n')
end

function format.lua_namespace_renum(env, moduledef, decl)
	if decl.attrs['r::hidden'] ~= nil then
		return
	end
	echo('local ', decl.commonname, ' = table.makeenum{\n')
	for name, value in pairs(decl.consts) do
		echo('\t[', q(name), '] = ', value, ',\n')
	end
	echo('}\n')
	echo('package.loaded[', q(decl.luaname), '] = ', decl.commonname, '\n')
end

function format.lua_namespace_rfield(env, moduledef, decl)
	if decl.attrs['r::hidden'] ~= nil then
		return
	end
	if decl.outerdecl then
		echo('\tfunction ', decl.outerdecl.commonname,
			':get', decl.lualocalname, '()\n')
	else
		echo('\tpackage.loaded[', q(decl.accessorluaname), '] = function()\n')
	end
	echo('\t\treturn ', decl.interntype:luaretprefix(),
		'r.', decl.commonname, '_accessor(')
	if decl.outerdecl and not decl.isstatic then
		if decl.outerdecl.classtype then
			echo('self.object')
		else
			echo('self')
		end
	end
	echo(')', decl.interntype:luaretsuffix(), '\n')
	echo('\tend\n')
	if decl.interntype:isimmutable() then
		return
	end
	if decl.outerdecl then
		echo('\tfunction ', decl.outerdecl.commonname, ':set', decl.lualocalname,
			'(value)\n')
	else
		echo('\tpackage.loaded[', q(decl.mutatorluaname), '] = function(value)\n')
	end
	echo('\t\tr.', decl.commonname, '_mutator(')
	if decl.outerdecl and not decl.isstatic then
		if decl.outerdecl.classtype then
			echo('self.object, ')
		else
			echo('self, ')
		end
	end
	echo(decl.interntype:luaargprefix(), 'value',
		decl.interntype:luaargsuffix(), ')\n')
	echo('\tend\n')
end

function format.lua_namespace_rmethod(env, moduledef, decl)
	if decl.attrs['r::hidden'] ~= nil then
		return
	end
	if decl.outerdecl then
		echo('function ', decl.outerdecl.commonname, ':',
			decl.lualocalname, '(')
	else
		echo('package.loaded[', q(decl.luaname), '] = function(')
	end
	local sep
	for i, arg in ipairs(decl.argtypes) do
		sep = sep_next(sep, ', ')
		echo(string.format('arg%i', i))
	end
	echo(')\n')
	if not decl.isnoexcept then
		echo('\tlocal exception = r_exceptionbox:new()\n')
	end
	echo('\t')
	if not decl.rettype.isvoid then
		echo('local result = ')
	end
	echo('r.', decl.commonname, '(')
	local sep
	if decl.outerdecl and not decl.isstatic then
		sep = sep_next(sep, ', ')
		if decl.outerdecl.classtype then
			echo('self.object')
		else
			echo('self')
		end
	end
	for i, arg in ipairs(decl.argtypes) do
		sep = sep_next(sep, ', ')
		echo(arg:luaargprefix(), string.format('arg%i', i), arg:luaargsuffix())
	end
	if not decl.isnoexcept then
		sep = sep_next(sep, ', ')
		echo('exception')
	end
	echo(')\n')
	if not decl.isnoexcept then
		echo('\tif exception.whatptr ~= nil then\n')
		echo('\t\terror(exception:what())\n')
		echo('\tend\n')
	end
	if not decl.rettype.isvoid then
		echo('\treturn ', decl.rettype:luaretprefix(), 'result',
			decl.rettype:luaretsuffix(), '\n')
	end
	echo('end\n')
end

function format.lua_init_rstruct(env, moduledef, decl)
	if decl.attrs['r::hidden'] ~= nil then
		return
	end
	echo(decl.commonname, ':buildmetatype()\n')
end

function format.lua_init_rclass(env, moduledef, decl)
	if decl.attrs['r::hidden'] ~= nil then
		return
	end
	echo(decl.commonname, ':buildmetatype()\n')
end

function format.lua(env, moduledef)
	echo([[
local rptr = ...
local ffi = require("ffi")
local r_exceptionbox = require("reflection.exceptionbox")
local r_wrapobject = require("reflection.wrapobject")
local r_unwrapobject = require("reflection.unwrapobject")
]])
	echo('ffi.cdef([[\n')
	for i, decl in ipairs(moduledef.decls) do
		echo(']] .. --[==[', decldefstring(decl), ']==] [[\n')
		if decl.rstruct then
			format.lua_ffi_rstruct(env, moduledef, decl)
		elseif decl.rclass then
			format.lua_ffi_rclass(env, moduledef, decl)
		elseif decl.rexternal then
			format.lua_ffi_rexternal(env, moduledef, decl)
		end
	end
	echo(']])\n')
	echo('local ptrtype = [[\n')
	echo('\tstruct{\n')
	for i, decl in ipairs(moduledef.decls) do
		echo(']] .. --[==[', decldefstring(decl), ']==] [[\n')
		if decl.rfield then
			format.lua_ptrtype_rfield(env, moduledef, decl)
		elseif decl.rmethod then
			format.lua_ptrtype_rmethod(env, moduledef, decl)
		end
	end
	echo('\t}*\n')
	echo(']]\n')
	echo('local r = ffi.cast(ptrtype, rptr)\n')
	for i, decl in ipairs(moduledef.decls) do
		echo('--[==[', decldefstring(decl), ']==]\n')
		if decl.rstruct then
			format.lua_namespace_rstruct(env, moduledef, decl)
		elseif decl.rclass then
			format.lua_namespace_rclass(env, moduledef, decl)
		elseif decl.renum then
			format.lua_namespace_renum(env, moduledef, decl)
		elseif decl.rfield then
			format.lua_namespace_rfield(env, moduledef, decl)
		elseif decl.rmethod then
			format.lua_namespace_rmethod(env, moduledef, decl)
		elseif decl.remit then
			echo(decl.payload)
		end
	end
	for i, decl in ipairs(moduledef.decls) do
		echo('--[==[', decldefstring(decl), ']==]\n')
		if decl.rstruct then
			format.lua_init_rstruct(env, moduledef, decl)
		elseif decl.rclass then
			format.lua_init_rclass(env, moduledef, decl)
		end
	end
end

function format.r_field_location(env, moduledef, decl)
	echo('\tlocation = {')
	local sep
	for i, entry in pairs(decl.location) do
		sep = sep_next(sep, ', ')
		echo('{', q(entry[1]), ', ', entry[2], ', ', entry[3], '}')
	end
	echo('},\n')
end

function format.r_field_attrs(env, moduledef, decl)
	echo('\tattrs = {')
	local sep
	for name, value in pairs(decl.attrs) do
		sep = sep_next(sep, ', ')
		echo('[', q(name), '] = ')
		if type(value) == 'table' then
			echo('{')
			local sep
			for i, arg in ipairs(value) do
				sep = sep_next(sep, ', ')
				echo(q(arg))
			end
			echo('}')
		else
			echo(value)
		end
	end
	echo('},\n')
end

function format.r_rstruct(env, moduledef, decl)
	echo('decl(moduledef, {\n')
	echo('\trstruct = true,\n')
	echo('\tstructtype = true,\n')
	format.r_field_location(env, moduledef, decl)
	format.r_field_attrs(env, moduledef, decl)
	echo('\tsourcename = ', q(decl.sourcename), ',\n')
	echo('\tluaname = ', q(decl.luaname), ',\n')
	echo('\tcommonname = ', q(decl.commonname), ',\n')
	echo('\tfields = {\n')
	for i, field in pairs(decl.fields) do
		echo('\t\t{\n')
		format.r_field_location(env, moduledef, field)
		format.r_field_attrs(env, moduledef, field)
		echo('\t\t\tname = {', q(field.name[1]), '},\n')
		echo('\t\t\tinterntype = ', field.interntype:serialize(), ',\n')
		echo('\t\t},\n')
	end
	echo('\t},\n')
	echo('})\n')
end

function format.r_rclass(env, moduledef, decl)
	echo('decl(moduledef, {\n')
	echo('\trclass = true,\n')
	echo('\tclasstype = true,\n')
	format.r_field_location(env, moduledef, decl)
	format.r_field_attrs(env, moduledef, decl)
	if decl.base then
		echo('\tbase = nslookup(moduledef, ', q(decl.base.sourcename), '),\n')
	end
	echo('\tsourcename = ', q(decl.sourcename), ',\n')
	echo('\tluaname = ', q(decl.luaname), ',\n')
	echo('\tcommonname = ', q(decl.commonname), ',\n')
	echo('})\n')
end

function format.r_rexternal(env, moduledef, decl)
	echo('decl(moduledef, {\n')
	echo('\trexternal = true,\n')
	echo('\tclasstype = true,\n')
	format.r_field_location(env, moduledef, decl)
	format.r_field_attrs(env, moduledef, decl)
	echo('\tsourcename = ', q(decl.sourcename), ',\n')
	echo('\tluaname = ', q(decl.luaname), ',\n')
	echo('\tcommonname = ', q(decl.commonname), ',\n')
	echo('})\n')
end

function format.r_renum(env, moduledef, decl)
	echo('decl(moduledef, {\n')
	echo('\trenum = true,\n')
	echo('\tenumtype = true,\n')
	format.r_field_location(env, moduledef, decl)
	format.r_field_attrs(env, moduledef, decl)
	echo('\tsourcename = ', q(decl.sourcename), ',\n')
	echo('\tluaname = ', q(decl.luaname), ',\n')
	echo('\tcommonname = ', q(decl.commonname), ',\n')
	echo('\tenumbase = ', q(decl.enumbase), ',\n')
	echo('\tconsts = {\n')
	for name, value in pairs(decl.consts) do
		echo('\t\t[', q(name), '] = ', value, ',\n')
	end
	echo('\t},\n')
	echo('})\n')
end

function format.r_rfield(env, moduledef, decl)
	echo('decl(moduledef, {\n')
	echo('\trfield = true,\n')
	format.r_field_location(env, moduledef, decl)
	format.r_field_attrs(env, moduledef, decl)
	if decl.outerdecl then
		echo('\touterdecl = nslookup(moduledef, ',
			q(decl.outerdecl.sourcename), '),\n')
	end
	echo('\tsourcename = ', q(decl.sourcename), ',\n')
	if decl.sourcelocalname then
		echo('\tsourcelocalname = ', q(decl.sourcelocalname), ',\n')
	end
	if decl.lualocalname then
		echo('\tlualocalname = ', q(decl.lualocalname), ',\n')
	end
	if decl.luaname then
		echo('\tluaname = ', q(decl.luaname), ',\n')
	end
	echo('\tcommonname = ', q(decl.commonname), ',\n')
	if decl.accessorluaname then
		echo('\taccessorluaname = ', q(decl.accessorluaname), ',\n')
	end
	if decl.mutatorluaname then
		echo('\tmutatorluaname = ', q(decl.mutatorluaname), ',\n')
	end
	if decl.selftype then
		echo('\tselftype = ', decl.selftype:serialize(), ',\n')
	end
	echo('\tinterntype = ', decl.interntype:serialize(), ',\n')
	echo('})\n')
end

function format.r_rmethod(env, moduledef, decl)
	echo('decl(moduledef, {\n')
	echo('\trmethod = true,\n')
	format.r_field_location(env, moduledef, decl)
	format.r_field_attrs(env, moduledef, decl)
	if decl.isnoexcept then
		echo('\tisnoexcept = true,\n')
	end
	echo('\trettype = ', decl.rettype:serialize(), ',\n')
	echo('\targtypes = {\n')
	for i, arg in ipairs(decl.argtypes) do
		echo('\t\t', arg:serialize(), ',\n')
	end
	echo('\t},\n')
	if decl.outerdecl then
		echo('\touterdecl = nslookup(moduledef, ',
			q(decl.outerdecl.sourcename), '),\n')
	end
	if decl.isconstructor then
		echo('\tisconstructor = true,\n')
	end
	if decl.isstatic then
		echo('\tisstatic = true,\n')
	end
	echo('\tsourcename = ', q(decl.sourcename), ',\n')
	if decl.sourcelocalname then
		echo('\tsourcelocalname = ', q(decl.sourcelocalname), ',\n')
	end
	if decl.lualocalname then
		echo('\tlualocalname = ', q(decl.lualocalname), ',\n')
	end
	if decl.luaname then
		echo('\tluaname = ', q(decl.luaname), ',\n')
	end
	echo('\tcommonname = ', q(decl.commonname), ',\n')
	if decl.selftype then
		echo('\tselftype = ', decl.selftype:serialize(), ',\n')
	end
	echo('})\n')
end

function format.r_remit(env, moduledef, decl)
	echo('decl(moduledef, {\n')
	echo('\tremit = true,\n')
	format.r_field_location(env, moduledef, decl)
	echo('\tpayload = ', q(decl.payload), ',\n')
	echo('})\n')
end

function format.r(env, moduledef)
	echo('local moduledef = registermodule(',
		q(moduledef.name), ', ', q(moduledef.source), ')\n')
	for i, include in ipairs(moduledef.includes) do
		echo('include(moduledef, ', q(include), ')\n')
	end
	for i, decl in ipairs(moduledef.decls) do
		if decl.rstruct then
			format.r_rstruct(env, moduledef, decl)
		elseif decl.rclass then
			format.r_rclass(env, moduledef, decl)
		elseif decl.rexternal then
			format.r_rexternal(env, moduledef, decl)
		elseif decl.renum then
			format.r_renum(env, moduledef, decl)
		elseif decl.rfield then
			format.r_rfield(env, moduledef, decl)
		elseif decl.rmethod then
			format.r_rmethod(env, moduledef, decl)
		elseif decl.remit then
			format.r_remit(env, moduledef, decl)
		end
	end
end

function format.indexcpp(env, modulelist, luaparts)
	echo('#include <reflection.hpp>\n')
	echo('#include <common.hpp>\n')
	for i, moduledef in ipairs(modulelist) do
		echo('extern r::module const r_', moduledef.identname, '_module;\n')
	end
	echo('namespace\n')
	echo('{\n')
	for i, part in ipairs(luaparts) do
		echo('\tuint8_t const r_lua', tostring(i), '_chunk[] = {')
		format.cpp_chunkbytes(env, part.chunk)
		echo('\t};\n')
		echo('\tr::module const r_lua', tostring(i), '_module = {\n')
		echo('\t\tnullptr,\n')
		echo('\t\t&r_lua', tostring(i), '_chunk,\n')
		echo('\t\tsizeof( r_lua', tostring(i), '_chunk ),\n')
		echo('\t\t', q(part.name), '};\n')
	end
	echo('}\n')
	for i = 1, #env.indexname - 1 do
		echo('namespace ', env.indexname[i], '\n')
		echo('{\n')
	end
	echo('\textern r::module const* ',
		env.indexname[#env.indexname], '[];\n')
	echo('\tr::module const* ',
		env.indexname[#env.indexname], '[] = {\n')
	for i, part in ipairs(luaparts) do
		if part.prolog then
			echo('\t\t&r_lua', tostring(i), '_module,\n')
		end
	end
	for i, moduledef in ipairs(modulelist) do
		echo('\t\t&r_', moduledef.identname, '_module,\n')
	end
	for i, part in ipairs(luaparts) do
		if part.epilog then
			echo('\t\t&r_lua', tostring(i), '_module,\n')
		end
	end
	echo('\t\tnullptr,\n')
	echo('\t};\n')
	for i = 1, #env.indexname - 1 do
		echo('}\n')
	end
end
