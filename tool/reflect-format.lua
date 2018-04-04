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

local function decltypestring(decltype, name, usesourcename)
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
		if decltype.isvolatile then
			part = part .. ' volatile'
		end
		return part
	end

	local function combine(basestr)
		return basestr .. prefix .. ' ' .. center .. suffix
	end

	while true do
		if usesourcename and decltype.sourcename then
			return combine(decltype.sourcename .. cvstr())
		elseif decltype.trivialname then
			return combine(decltype.trivialname .. cvstr())
		elseif decltype.classname then
			return combine(decltype.classname .. cvstr())
		elseif decltype.fundamental then
			return combine(decltype.fundamental .. cvstr())
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

local function decltypesourcestring(decltype, name)
	return decltypestring(decltype, name, true)
end

local function istrivialtype(decltype)
	if decltype.trivialname then
		return true
	elseif decltype.fundamental then
		return true
	elseif decltype.pointer then
		return istrivialtype(decltype.pointer)
	elseif decltype.array then
		if not decltype.size then
			return false
		end
		return istrivialtype(decltype.array)
	else
		return false
	end
end

local function asserttrivialtype(decltype, errorlocation)
	if not istrivialtype(decltype) then
		parser.locationerror(errorlocation,
			'failed to reflect a non-trivial value of type ' ..
				decltypesourcestring(decltype))
	end
end

function format.cpp_namespace_rfield(env, moduledef, decl)
	echo('\textern "C" ', decltypesourcestring(decl.interntype),
		'r_', decl.cname, '_accessor(')
	if decl.outerdecl and not decl.isstatic then
		echo(decltypesourcestring(decl.selftype, 'self'))
	end
	echo(')\n')
	echo('\t{\n')
	echo('\t\treturn ')
	if decl.outerdecl and not decl.isstatic then
		echo('self->', decl.sourcelocalname)
	else
		echo(decl.sourcename)
	end
	echo(';\n')
	echo('\t}\n')
	if decl.isreadonly then
		return
	end
	echo('\textern "C" void r_', decl.cname, '_mutator(')
	if decl.outerdecl and not decl.isstatic then
		echo(decltypesourcestring(decl.selftype, 'self'), ', ')
	end
	echo(decltypesourcestring(decl.interntype, 'value'))
	echo(')\n')
	echo('\t{\n')
	echo('\t\t')
	if decl.outerdecl and not decl.isstatic then
		echo('self->', decl.sourcelocalname)
	else
		echo(decl.sourcename)
	end
	echo(' = value;\n')
	echo('\t}\n')
end

function format.cpp_namespace_rmethod(env, moduledef, decl)
	echo('\textern "C" bool r_', decl.cname, '_wrapper(')
	local sep
	if decl.outerdecl and not decl.isstatic then
		sep = sep_next(sep, ', ')
		echo(decltypesourcestring(decl.selftype, 'self'))
	end
	for i, argtype in ipairs(decl.argtypes) do
		sep = sep_next(sep, ', ')
		echo(decltypesourcestring(argtype, string.format('arg%i', i)))
	end
	if decl.rettype.fundamental ~= 'void' then
		sep = sep_next(sep, ', ')
		local ptrtype = {
			pointer = decl.rettype}
		echo(decltypesourcestring(ptrtype, 'result'))
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
	if decl.rettype.fundamental ~= 'void' then
		echo('*result = ')
	end
	if decl.outerdecl and not decl.isstatic then
		echo('self->', decl.sourcelocalname)
	elseif decl.isconstructor then
		echo('new ', decl.outerdecl.sourcename)
	else
		echo(decl.sourcename)
	end
	if not decl.isconstructor or #decl.argtypes ~= 0 then
		echo('(')
		local sep
		for i, argtype in ipairs(decl.argtypes) do
			sep = sep_next(sep, ', ')
			echo(string.format('arg%i', i))
		end
		echo(')')
	end
	echo(';\n')
	echo('\t\t\treturn true;\n')
	if not decl.isnoexcept then
		echo('\t\t}\n')
		echo('\t\tcatch(std::exception const& e)\n')
		echo('\t\t{\n')
		echo('\t\t\tr::exceptionbox::make(exception);\n')
		echo('\t\t\treturn false;\n')
		echo('\t\t}\n')
	end
	echo('\t}\n')
end

function format.cpp_ptrtype_rfield(env, moduledef, decl)
	echo('\t\tdecltype(&r_', decl.cname, '_accessor) ',
		decl.cname, '_accessor', ';\n')
	if not decl.isreadonly then
		echo('\t\tdecltype(&r_', decl.cname, '_mutator) ',
			decl.cname, '_mutator', ';\n')
	end
end

function format.cpp_ptrtype_robject(env, moduledef, decl)
	echo('\t\tdecltype(&', decl.sourcename, ') ', decl.cname, ';\n')
end

function format.cpp_ptrtype_rmethod(env, moduledef, decl)
	echo('\t\tdecltype(&r_', decl.cname, '_wrapper) ', decl.cname, ';\n')
end

function format.cpp_ptrinit_rfield(env, moduledef, decl)
	echo('\t\t&r_', decl.cname, '_accessor,\n')
	if not decl.isreadonly then
		echo('\t\t&r_', decl.cname, '_mutator,\n')
	end
end

function format.cpp_ptrinit_robject(env, moduledef, decl)
	echo('\t\t&', decl.sourcename, ',\n')
end

function format.cpp_ptrinit_rmethod(env, moduledef, decl)
	echo('\t\t&r_', decl.cname, '_wrapper,\n')
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
	echo('\tstruct r_ptr_t {\n')
	for i, decl in ipairs(moduledef.decls) do
		echo('/*', decldefstring(decl), '*/\n')
		if decl.rfield then
			format.cpp_ptrtype_rfield(env, moduledef, decl)
		elseif decl.robject then
			format.cpp_ptrtype_robject(env, moduledef, decl)
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
		elseif decl.robject then
			format.cpp_ptrinit_robject(env, moduledef, decl)
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
	echo('\t', q(moduledef.name .. '.r.lua'), '};\n')
end

function format.lua_wrapvalue(env, varname, decltype, errorlocation)
	if istrivialtype(decltype) then
	elseif decltype.pointer and decltype.pointer.classname then
		echo('\t\t\tif ', varname, ' ~= nil then\n')
		echo('\t\t\t\t', varname, ' = ffi.new(',
			q('r_' .. decltype.pointer.classname .. '_box'), ', ',
			varname, ')\n')
		echo('\t\t\telse\n')
		echo('\t\t\t\t', varname, ' = nil\n')
		echo('\t\t\tend\n')
	else
		parser.locationerror(errorlocation,
			'invalid value type ' .. decltypesourcestring(decltype))
	end
end

function format.lua_unwrapvalue(env, varname, decltype, errorlocation)
	if istrivialtype(decltype) then
	elseif decltype.pointer and decltype.pointer.classname then
		echo('\t\t\tif ', varname, ' then\n')
		if not env.suppresstypechecks then
			echo('\t\t\t\tif type(', varname, ') ~= "cdata" or not ',
				varname, '[', q('#' .. decltype.pointer.classluaname), '] then\n')
			echo('\t\t\t\t\terror(',
				q('expected an object of type ' ..
					decltype.pointer.classluaname .. ' for ' .. varname), ')\n')
			echo('\t\t\t\tend\n')
		end
		echo('\t\t\t\t', varname, ' = ', varname, '.ptr\n')
		echo('\t\t\tend\n')
	else
		parser.locationerror(errorlocation,
			'invalid value type ' .. decltypesourcestring(decltype))
	end
	if decltype.isrequired and not env.suppresstypechecks then
		echo('\t\t\tif ', varname, ' == nil then\n')
		echo('\t\t\t\terror(',
			q(varname .. ' is required to be non-null'), ')\n')
		echo('\t\t\tend\n')
	end
end

function format.lua_ffi_rtype(env, moduledef, decl)
	asserttrivialtype(decl.interntype, decl.location)
	echo('\ttypedef ', decltypestring(decl.interntype, decl.cname), ';\n')
end

function format.lua_ffi_rstruct(env, moduledef, decl)
	echo('\ttypedef struct ', decl.cname, ' {\n')
	for i, field in ipairs(decl.fields) do
		asserttrivialtype(field.interntype, field.location)
		echo('\t\t', decltypestring(field.interntype, field.name[1]), ';\n')
	end
	echo('\t} ', decl.cname, ';\n')
end

function format.lua_ffi_rclass(env, moduledef, decl)
	echo('\ttypedef void ', decl.classname, ';\n')
	echo('\ttypedef struct r_', decl.classname, '_box {\n')
	echo('\t\t', decl.classname, '* ptr;\n')
	echo('\t} r_', decl.classname, '_box;\n')
end

function format.lua_ffi_rexternal(env, moduledef, decl)
	echo('\ttypedef void ', decl.classname, ';\n')
end

function format.lua_ptrtype_rfield(env, moduledef, decl)
	echo('\t\t', decltypestring(decl.interntype),
		'(*', decl.cname, '_accessor)(')
	if decl.outerdecl and not decl.isstatic then
		echo(decltypestring(decl.selftype, 'self'))
	end
	echo(');\n')
	if decl.isreadonly then
		return
	end
	echo('\t\tvoid(*', decl.cname, '_mutator)(')
	if decl.outerdecl and not decl.isstatic then
		echo(decltypestring(decl.selftype, 'self'), ', ')
	end
	echo(decltypestring(decl.interntype, 'value'))
	echo(');\n')
end

function format.lua_ptrtype_robject(env, moduledef, decl)
	local fieldtype = {
		pointer = decl.interntype}
	echo('\t\t', decltypestring(fieldtype, decl.cname), ';\n')
end

function format.lua_ptrtype_rmethod(env, moduledef, decl)
	echo('\t\tbool(*', decl.cname, ')(')
	local sep
	if decl.outerdecl and not decl.isstatic then
		sep = sep_next(sep, ', ')
		echo(decltypestring(decl.selftype, 'self'))
	end
	for i, argtype in ipairs(decl.argtypes) do
		sep = sep_next(sep, ', ')
		echo(decltypestring(argtype, string.format('arg%i', i)))
	end
	if decl.rettype.fundamental ~= 'void' then
		sep = sep_next(sep, ', ')
		local ptrtype = {
			pointer = decl.rettype}
		echo(decltypestring(ptrtype, 'result'))
	end
	if not decl.isnoexcept then
		sep = sep_next(sep, ', ')
		echo('r_exception_t* exception')
	end
	echo(');\n')
end

function format.lua_namespace_rtype(env, moduledef, decl)
end

function format.lua_namespace_rstruct(env, moduledef, decl)
	if decl.attrs['r::hidden'] ~= nil then
		return
	end
	echo('local ', decl.cname,
		' = package.loaded["base.ffipure"]:module(', q(decl.luaname), ')\n')
	echo(decl.cname,
		'.typedef = ', q(decl.cname), '\n')
end

function format.lua_namespace_rclass(env, moduledef, decl)
	if decl.attrs['r::hidden'] ~= nil then
		return
	end
	echo('local ', decl.classname, ' = package.loaded[')
	if decl.base then
		echo(q(decl.base.luaname))
	else
		echo('"base.ffipure"')
	end
	echo(']:module(', q(decl.luaname), ')\n')
	echo(decl.classname,
		'.typedef = ', q('r_' .. decl.classname .. '_box'), '\n')
end

function format.lua_namespace_renum(env, moduledef, decl)
	if decl.attrs['r::hidden'] ~= nil then
		return
	end
	echo('local ', decl.cname, ' = table.makeenum{\n')
	for name, value in pairs(decl.consts) do
		echo('\t[', q(name), '] = ', value, ',\n')
	end
	echo('}\n')
	echo('package.loaded[', q(decl.luaname), '] = ', decl.cname, '\n')
end

function format.lua_namespace_rfield(env, moduledef, decl)
	if decl.attrs['r::hidden'] ~= nil then
		return
	end
	if decl.outerdecl then
		echo('\tfunction ', decl.outerdecl.cname,
			':get', decl.lualocalname, '()\n')
	else
		echo('\tpackage.loaded[', q(decl.accessorluaname), '] = function()\n')
	end
	echo('\t\tlocal result = r.', decl.cname, '_accessor(')
	if decl.outerdecl and not decl.isstatic then
		echo('self.ptr')
	end
	echo(')\n')
	format.lua_wrapvalue(env, 'result', decl.interntype, decl.location)
	echo('\t\treturn result\n')
	echo('\tend\n')
	if decl.isreadonly then
		return
	end
	if decl.outerdecl then
		echo('\tfunction ', decl.outerdecl.cname, ':set', decl.lualocalname,
			'(value)\n')
	else
		echo('\tpackage.loaded[', q(decl.mutatorluaname), '] = function(value)\n')
	end
	format.lua_unwrapvalue(env, 'value', decl.interntype, decl.location)
	echo('\t\tr.', decl.cname, '_mutator(')
	if decl.outerdecl and not decl.isstatic then
		echo('self.ptr, ')
	end
	echo('value)\n')
	echo('\tend\n')
end

function format.lua_namespace_robject(env, moduledef, decl)
	if decl.attrs['r::hidden'] ~= nil then
		return
	end
	if decl.interntype.classname then
		echo('local ', decl.cname, ' = ffi.gc(ffi.new(',
			q('r_' .. decl.interntype.classname .. '_box'),
			', ffi.cast(', q(decl.interntype.classname .. '*'),
			', r.', decl.cname, ')), nil)\n')
		echo('package.loaded[', q(decl.luaname), '] = ', decl.cname, '\n')
	end
end

function format.lua_namespace_rmethod(env, moduledef, decl)
	if decl.attrs['r::hidden'] ~= nil then
		return
	end
	echo('do\n')
	if decl.rettype.fundamental ~= 'void' then
		local arrtype = {
			array = decl.rettype,
			size = 1}
		echo('\tlocal result_t = ffi.typeof(',
			q(decltypestring(arrtype)), ')\n')
	end
	if decl.outerdecl then
		echo('\tfunction ', decl.outerdecl.cname, ':', decl.lualocalname, '(')
	else
		echo('\tpackage.loaded[', q(decl.luaname), '] = function(')
	end
	local sep
	for i, argtype in ipairs(decl.argtypes) do
		sep = sep_next(sep, ', ')
		echo(string.format('arg%i', i))
	end
	echo(')\n')
	if decl.rettype.fundamental ~= 'void' then
		echo('\t\tlocal result = result_t()\n')
	end
	if not decl.isnoexcept then
		echo('\t\tlocal exception = r_exception()\n')
	end
	for i, argtype in ipairs(decl.argtypes) do
		format.lua_unwrapvalue(
			env, string.format('arg%i', i), argtype, decl.location)
	end
	echo('\t\t')
	if not decl.isnoexcept then
		echo('if ')
	end
	echo('r.', decl.cname, '(')
	local sep
	if decl.outerdecl and not decl.isstatic then
		sep = sep_next(sep, ', ')
		echo('self.ptr')
	end
	for i, argtype in ipairs(decl.argtypes) do
		sep = sep_next(sep, ', ')
		echo(string.format('arg%i', i))
	end
	if decl.rettype.fundamental ~= 'void' then
		sep = sep_next(sep, ', ')
		echo('result')
	end
	if not decl.isnoexcept then
		sep = sep_next(sep, ', ')
		echo('exception')
	end
	echo(')')
	if not decl.isnoexcept then
		echo(' then')
	end
	echo('\n')
	if decl.rettype.fundamental ~= 'void' then
		echo('\t\t\tresult = result[0]\n')
		format.lua_wrapvalue(env, 'result', decl.rettype, decl.location)
		echo('\t\t\treturn result\n')
	else
		echo('\t\t\treturn\n')
	end
	if not decl.isnoexcept then
		echo('\t\telse\n')
		echo('\t\t\terror(ffi.string(exception.what(exception)))\n')
		echo('\t\tend\n')
	end
	echo('\tend\n')
	echo('end\n')
end

function format.lua_init_rstruct(env, moduledef, decl)
	if decl.attrs['r::hidden'] ~= nil then
		return
	end
	echo(decl.cname, ':buildmetatype()\n')
end

function format.lua_init_rclass(env, moduledef, decl)
	if decl.attrs['r::hidden'] ~= nil then
		return
	end
	echo(decl.classname, ':buildmetatype()\n')
end

function format.lua(env, moduledef)
	echo([[
local rptr = ...
local ffi = require("ffi")
local r_exception = package.loaded["r_exception"]
if not r_exception then
	ffi.cdef([==[
		typedef struct r_exception_t{
			void(*free)(struct r_exception_t* self);
			char const*(*what)(struct r_exception_t* self);
			void* data[4];
		} r_exception_t;
	]==])
	local function r_exception_gc(self)
		if self.free ~= nil then
			self.free(self)
		end
	end
	r_exception = function()
		return ffi.gc(ffi.new("r_exception_t"), r_exception_gc)
	end
	package.loaded["r_exception"] = r_exception
end
]])
	echo('ffi.cdef([[\n')
	for i, decl in ipairs(moduledef.decls) do
		echo(']] .. --[==[', decldefstring(decl), ']==] [[\n')
		if decl.rtype then
			format.lua_ffi_rtype(env, moduledef, decl)
		elseif decl.rstruct then
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
		elseif decl.robject then
			format.lua_ptrtype_robject(env, moduledef, decl)
		elseif decl.rmethod then
			format.lua_ptrtype_rmethod(env, moduledef, decl)
		end
	end
	echo('\t}*\n')
	echo(']]\n')
	echo('local r = ffi.cast(ptrtype, rptr)\n')
	for i, decl in ipairs(moduledef.decls) do
		echo('--[==[', decldefstring(decl), ']==]\n')
		if decl.rtype then
			format.lua_namespace_rtype(env, moduledef, decl)
		elseif decl.rstruct then
			format.lua_namespace_rstruct(env, moduledef, decl)
		elseif decl.rclass then
			format.lua_namespace_rclass(env, moduledef, decl)
		elseif decl.renum then
			format.lua_namespace_renum(env, moduledef, decl)
		elseif decl.rfield then
			format.lua_namespace_rfield(env, moduledef, decl)
		elseif decl.robject then
			format.lua_namespace_robject(env, moduledef, decl)
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

function format.r_interntype(env, moduledef, decltype)
	echo('{')
	if decltype.pointer then
		echo('pointer = ')
		format.r_interntype(env, moduledef, decltype.pointer)
	elseif decltype.array then
		echo('array = ')
		format.r_interntype(env, moduledef, decltype.array)
		echo(', size = ', decltype.size)
	else
		local sep
		if decltype.sourcename then
			sep = sep_next(sep, ', ')
			echo('sourcename = ', q(decltype.sourcename))
		end
		if decltype.trivialname then
			sep = sep_next(sep, ', ')
			echo('trivialname = ', q(decltype.trivialname))
		end
		if decltype.classname then
			sep = sep_next(sep, ', ')
			echo('classname = ', q(decltype.classname))
		end
		if decltype.fundamental then
			sep = sep_next(sep, ', ')
			echo('fundamental = ', q(decltype.fundamental))
		end
		if not sep then
			error('unknown type category')
		end
	end
	if decltype.isconst then
		echo(', isconst = true')
	end
	if decltype.isvolatile then
		echo(', isvolatile = true')
	end
	if decltype.isrequired then
		echo(', isrequired = true')
	end
	echo('}')
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

function format.r_rtype(env, moduledef, decl)
	echo('decl(moduledef, {\n')
	echo('\trtype = true,\n')
	format.r_field_location(env, moduledef, decl)
	format.r_field_attrs(env, moduledef, decl)
	echo('\tsourcename = ', q(decl.sourcename), ',\n')
	echo('\tluaname = ', q(decl.luaname), ',\n')
	echo('\tcname = ', q(decl.cname), ',\n')
	echo('\ttrivialname = ', q(decl.trivialname), ',\n')
	echo('\tinterntype = ')
	format.r_interntype(env, moduledef, field.interntype)
	echo(',\n')
	echo('})\n')
end

function format.r_rstruct(env, moduledef, decl)
	echo('decl(moduledef, {\n')
	echo('\trstruct = true,\n')
	format.r_field_location(env, moduledef, decl)
	format.r_field_attrs(env, moduledef, decl)
	echo('\tsourcename = ', q(decl.sourcename), ',\n')
	echo('\tluaname = ', q(decl.luaname), ',\n')
	echo('\tcname = ', q(decl.cname), ',\n')
	echo('\ttrivialname = ', q(decl.trivialname), ',\n')
	echo('\tfields = {\n')
	for i, field in pairs(decl.fields) do
		echo('\t\t{\n')
		format.r_field_location(env, moduledef, field)
		format.r_field_attrs(env, moduledef, field)
		echo('\t\t\tname = {', q(field.name[1]), '},\n')
		echo('\t\t\tinterntype = ')
		format.r_interntype(env, moduledef, field.interntype)
		echo(',\n')
		echo('\t\t},\n')
	end
	echo('\t},\n')
	echo('})\n')
end

function format.r_rclass(env, moduledef, decl)
	echo('decl(moduledef, {\n')
	echo('\trclass = true,\n')
	format.r_field_location(env, moduledef, decl)
	format.r_field_attrs(env, moduledef, decl)
	if decl.base then
		echo('\tbase = nslookup(moduledef, ', q(decl.base.sourcename), '),\n')
	end
	echo('\tsourcename = ', q(decl.sourcename), ',\n')
	echo('\tluaname = ', q(decl.luaname), ',\n')
	echo('\tcname = ', q(decl.cname), ',\n')
	echo('\tclassname = ', q(decl.classname), ',\n')
	echo('})\n')
end

function format.r_rexternal(env, moduledef, decl)
	echo('decl(moduledef, {\n')
	echo('\trexternal = true,\n')
	format.r_field_location(env, moduledef, decl)
	format.r_field_attrs(env, moduledef, decl)
	echo('\tsourcename = ', q(decl.sourcename), ',\n')
	echo('\tluaname = ', q(decl.luaname), ',\n')
	echo('\tcname = ', q(decl.cname), ',\n')
	echo('\tclassname = ', q(decl.classname), ',\n')
	echo('})\n')
end

function format.r_renum(env, moduledef, decl)
	echo('decl(moduledef, {\n')
	echo('\trenum = true,\n')
	format.r_field_location(env, moduledef, decl)
	format.r_field_attrs(env, moduledef, decl)
	echo('\tsourcename = ', q(decl.sourcename), ',\n')
	echo('\tluaname = ', q(decl.luaname), ',\n')
	echo('\tcname = ', q(decl.cname), ',\n')
	echo('\tfundamentaleq = ', q(decl.fundamentaleq), ',\n')
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
	echo('\tcname = ', q(decl.cname), ',\n')
	if decl.accessorluaname then
		echo('\taccessorluaname = ', q(decl.accessorluaname), ',\n')
	end
	if decl.mutatorluaname then
		echo('\tmutatorluaname = ', q(decl.mutatorluaname), ',\n')
	end
	if decl.selftype then
		echo('\tselftype = ')
		format.r_interntype(env, moduledef, decl.selftype)
		echo(',\n')
	end
	echo('\tinterntype = ')
	format.r_interntype(env, moduledef, decl.interntype)
	echo(',\n')
	if decl.isreadonly then
		echo('\tisreadonly = true,\n')
	end
	echo('})\n')
end

function format.r_robject(env, moduledef, decl)
	echo('decl(moduledef, {\n')
	echo('\trobject = true,\n')
	format.r_field_location(env, moduledef, decl)
	format.r_field_attrs(env, moduledef, decl)
	echo('\tsourcename = ', q(decl.sourcename), ',\n')
	echo('\tluaname = ', q(decl.luaname), ',\n')
	echo('\tcname = ', q(decl.cname), ',\n')
	echo('\tinterntype = ')
	format.r_interntype(env, moduledef, decl.interntype)
	echo(',\n')
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
	echo('\trettype = ')
	format.r_interntype(env, moduledef, decl.rettype)
	echo(',\n')
	echo('\targtypes = {\n')
	for i, arg in ipairs(decl.argtypes) do
		echo('\t\t')
		format.r_interntype(env, moduledef, arg)
		echo(',\n')
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
	echo('\tcname = ', q(decl.cname), ',\n')
	if decl.selftype then
		echo('\tselftype = ')
		format.r_interntype(env, moduledef, decl.selftype)
		echo(',\n')
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
		if decl.rtype then
			format.r_rtype(env, moduledef, decl)
		elseif decl.rstruct then
			format.r_rstruct(env, moduledef, decl)
		elseif decl.rclass then
			format.r_rclass(env, moduledef, decl)
		elseif decl.rexternal then
			format.r_rexternal(env, moduledef, decl)
		elseif decl.renum then
			format.r_renum(env, moduledef, decl)
		elseif decl.rfield then
			format.r_rfield(env, moduledef, decl)
		elseif decl.robject then
			format.r_robject(env, moduledef, decl)
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
