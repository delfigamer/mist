local modname = ...
local format = package.modtable(modname)

local function echo(...)
	local arg = table.pack(...)
	for i = 1, arg.count do
		if arg[i] ~= nil then
			coroutine.yield(tostring(arg[i]))
		end
	end
end

local function sep_next(prev, sep)
	if prev then
		echo(sep)
	end
	return true
end

function format.format(func, ...)
	local content = {}
	local thread = coroutine.create(func)
	local suc, ret = coroutine.resume(thread, ...)
	while true do
		if not suc then
			error(ret)
		elseif ret then
			table.append(content, ret)
		end
		if coroutine.status(thread) == 'suspended' then
			suc, ret = coroutine.resume(thread)
		else
			break
		end
	end
	return table.concat(content)
end

function format.emit_point(emit_ents, target)
	for i, ent in ipairs(emit_ents) do
		if ent.target == target then
			echo(ent.content)
		end
	end
end

function format.type_cstr(typedef)
	if typedef.type == 'void' then
		echo('void')
	elseif typedef.type == 'reference' then
		echo(typedef.target.fcname)
	elseif typedef.type == 'pointer' then
		format.type_cstr(typedef.base)
		echo('*')
	elseif typedef.type == 'classbox' then
		echo(typedef.target.fcname, '*')
	end
	if typedef.bconst then
		echo(' const')
	end
end

function format.type_ffistr(typedef)
	if typedef.type == 'void' then
		echo('void')
	elseif typedef.type == 'reference' then
		echo(typedef.target.ffiname)
	elseif typedef.type == 'pointer' then
		format.type_ffistr(typedef.base)
		echo('*')
	elseif typedef.type == 'classbox' then
		echo(typedef.target.ffiname, '*')
	end
	if typedef.bconst then
		echo(' const')
	end
end

function format.enum_luadef(ent)
	echo('local ', ent.lname, ' = table.makeenum{\n')
	for i, item in ipairs(ent.items) do
		echo('\t[\'', item.name, '\'] = ', item.value, ',\n')
	end
	echo('}\npackage.modtable(\'host.', ent.lname, '\', ', ent.lname, ')\n')
end

function format.struct_ffidef(ent)
	echo('struct ', ent.lname, ' {\n')
	for i, field in ipairs(ent.fields) do
		echo('\t')
		format.type_ffistr(field.type)
		echo(' ', field.name)
		if field.arrsize then
			echo('[', tostring(field.arrsize), ']')
		end
		echo(';\n')
	end
	echo('};\n')
end

function format.method_carglist(ent)
	echo('(')
	local sep
	if not ent.bstatic then
		sep = sep_next(sep, ', ')
		echo(ent.class.fcname, ent.attrs.const and ' const', '* self')
	end
	for i, arg in ipairs(ent.arglist) do
		sep = sep_next(sep, ', ')
		format.type_cstr(arg.type)
		echo(' ', arg.name)
	end
	if ent.rettype.type ~= 'void' then
		sep = sep_next(sep, ', ')
		format.type_cstr(ent.rettype)
		echo('* result')
	end
	echo(')')
end

function format.method_cbody(ent)
	if not ent.attrs.noexcept then
		echo('\ttry\n')
	end
	echo('\t{\n')
	echo('\t\t')
	if ent.rettype.type ~= 'void' then
		echo('*result = ')
	end
	if ent.bstatic then
		echo(ent.class.fcname, '::', ent.cname)
	else
		echo('self->', ent.cname)
	end
	echo('(')
	local sep
	for i, arg in ipairs(ent.arglist) do
		sep = sep_next(sep, ', ')
		echo(arg.name)
	end
	echo(');\n')
	echo('\t\treturn true;\n')
	echo('\t}\n')
	if not ent.attrs.noexcept then
		echo('\tcatch(::std::exception const& e)\n\z
			\t{\n\z
			\t\t::utils::cbase::seterror(e.what());\n\z
			\t\treturn false;\n\z
			\t}\n\z
			\tcatch(...)\n\z
			\t{\n\z
			\t\t::utils::cbase::seterror("unknown exception");\n\z
			\t\treturn false;\n\z
			\t}\n')
	end
end

function format.method_ffiarglist(ent)
	echo('(')
	local sep
	if not ent.bstatic then
		sep = sep_next(sep, ', ')
		echo(ent.class.lname, ent.attrs.const and ' const', '* self')
	end
	for i, arg in ipairs(ent.arglist) do
		sep = sep_next(sep, ', ')
		format.type_ffistr(arg.type)
		echo(' ', arg.name)
	end
	if ent.rettype.type ~= 'void' then
		sep = sep_next(sep, ', ')
		format.type_ffistr(ent.rettype)
		echo('* result')
	end
	echo(')')
end

function format.method_luaarglist(ent)
	echo('(')
	local sep
	for i, arg in ipairs(ent.arglist) do
		sep = sep_next(sep, ', ')
		echo(arg.name)
	end
	echo(')')
end

function format.method_luabody(ent)
	if ent.rettype.type ~= 'void' then
		echo('\tlocal result = ffi.new(\'')
		format.type_ffistr(ent.rettype)
		echo('[1]\')\n')
	end
	echo('\t')
	if not ent.attrs.noexcept then
		echo('if ')
	end
	echo('methodlist.', ent.flname, '(')
	do
		local sep
		if not ent.bstatic then
			sep = sep_next(sep, ', ')
				echo('self.ptr')
		end
		for i, arg in ipairs(ent.arglist) do
			sep = sep_next(sep, ', ')
			if arg.type.type == 'classbox' then
				echo(arg.name, ' and ', arg.name, '.ptr')
			else
				echo(arg.name)
			end
		end
		if ent.rettype.type ~= 'void' then
			sep = sep_next(sep, ', ')
			echo('result')
		end
	end
	echo(')')
	if not ent.attrs.noexcept then
		echo(' then')
	end
	echo('\n')
	if ent.meta.addref then
		echo('\t\treference_addref(result[0])\n')
	end
	echo('\t\treturn')
	if ent.rettype.type == 'classbox' then
		echo(' result[0] ~= nil and ',
			ent.rettype.target.lname, ':new(result[0])')
	elseif ent.rettype.type ~= 'void' then
		if ent.meta.stringwrap then
			echo(' result[0] ~= nil and ffi.string(result[0])')
		elseif ent.meta.gc then
			echo(' result[0] ~= nil and ffi.gc(result[0], ', ent.meta.gc, ')')
		else
			echo(' result[0]')
		end
	end
	echo('\n')
	if not ent.attrs.noexcept then
		echo('\telse\n')
		echo('\t\terror(cbase:geterror())\n')
		echo('\tend\n')
	end
end

function format.hpp(t)
	format.emit_point(t.emit_ents, 'hpp_start')
	echo('#pragma once\n')
	format.emit_point(t.emit_ents, 'hpp_beforeincludes')
	echo('#include <common.hpp>\n')
	echo('#include <cinttypes>\n')
	format.emit_point(t.emit_ents, 'hpp_beforeclasses')
	for i, ent in ipairs(t.type_ents) do
		if ent.type == 'class' or
			ent.type == 'struct' or
			ent.type == 'enum'
		then
			for ns in string.gmatch(ent.nsprefix, '[^:]+') do
				echo('namespace ', ns, ' {')
			end
			echo(ent.keyword, ' ', ent.cname, ';')
			for ns in string.gmatch(ent.nsprefix, '[^:]+') do
				echo('}')
			end
			echo('\n')
		end
	end
	format.emit_point(t.emit_ents, 'hpp_mldecl')
	echo('struct ', t.structname, '_t\n')
	echo('{\n')
	format.emit_point(t.emit_ents, 'hpp_mldeclstart')
	for i, ent in ipairs(t.method_ents) do
		echo('\tbool(*', ent.flname, ')')
		format.method_carglist(ent)
		echo(' NOEXCEPT;\n')
	format.emit_point(t.emit_ents, 'hpp_mldeclend')
	end
	format.emit_point(t.emit_ents, 'hpp_beforemldef')
	echo('};\n')
	echo('extern ', t.structname, '_t const ', t.structname, ';\n')
	format.emit_point(t.emit_ents, 'hpp_end')
end

function format.cpp(t)
	format.emit_point(t.emit_ents, 'cpp_start')
	echo('#include <', t.fileprefix, '.hpp>\n')
	for i, header in ipairs(t.boundheaders) do
		echo('#include <', header, '>\n')
	end
	echo('#include <exception>\n')
	echo('namespace {\n')
	format.emit_point(t.emit_ents, 'cpp_beforemethods')
	local sep
	for i, ent in ipairs(t.method_ents) do
		sep = sep_next(sep, '\n')
		echo('\tbool ', ent.flname)
		format.method_carglist(ent)
		echo(' NOEXCEPT\n')
		format.method_cbody(ent)
	end
	echo('}\n')
	format.emit_point(t.emit_ents, 'cpp_beforemldef')
	echo(t.structname, '_t const ', t.structname, ' = {\n')
	for i, ent in ipairs(t.method_ents) do
		echo('\t', ent.flname, ',\n')
	end
	echo('};\n')
	format.emit_point(t.emit_ents, 'cpp_end')
end

function format.ffi(t)
	format.emit_point(t.emit_ents, 'ffi_start')
	echo('struct ', t.structname, '_t;\n')
	echo('typedef void* ')
	do
		local sep
		for i, ent in ipairs(t.type_ents) do
			if ent.type == 'class' then
				sep = sep_next(sep, ', ')
				echo(ent.lname)
			end
		end
	end
	echo(';\n')
	for i, ent in ipairs(t.type_ents) do
		if ent.type == 'struct' then
			echo('struct ', ent.lname, ';\n')
		end
	end
	for i, ent in ipairs(t.type_ents) do
		if ent.type == 'struct' then
			format.struct_ffidef(ent)
		end
	end
	format.emit_point(t.emit_ents, 'ffi_beforemldecl')
	echo('struct ', t.structname, '_t {\n')
	format.emit_point(t.emit_ents, 'ffi_mldeclstart')
	for i, ent in ipairs(t.method_ents) do
		echo('\tbool(*', ent.flname, ')')
		format.method_ffiarglist(ent)
		echo(';\n')
	end
	format.emit_point(t.emit_ents, 'ffi_mldeclend')
	echo('};\n')
	format.emit_point(t.emit_ents, 'ffi_end')
end

function format.lua(t)
	format.emit_point(t.emit_ents, 'lua_start')
	echo('local ffi = require(\'ffi\')\nffi.cdef[[')
	local ffistr = format.format(format.ffi, t)
	if t.compactffi then
		ffistr = string.match(ffistr, '^%s*(.-)%s*$')
		ffistr = string.gsub(ffistr, '%s*([^%w_])%s*', '%1')
		ffistr = string.gsub(ffistr, '%s+', ' ')
	end
	echo(ffistr)
	echo(']]\n')
	format.emit_point(t.emit_ents, 'lua_beforeclasses')
	echo('local ffipure = require(\'base.ffipure\')\n')
	for i, ent in ipairs(t.type_ents) do
		if ent.type == 'class' or ent.type == 'struct' then
			if ent.parent then
				echo('local ', ent.lname, ' = require(\'host.',
					ent.parent.lname, '\'):module(\'host.', ent.lname, '\')\n')
			else
				echo('local ', ent.lname, ' = ffipure:module(\'host.',
					ent.lname, '\')\n')
			end
		elseif ent.type == 'enum' then
			format.enum_luadef(ent)
		end
	end
	format.emit_point(t.emit_ents, 'lua_beforemethods')
	local sep
	for i, ent in ipairs(t.method_ents) do
		if not ent.meta.noluamethod then
			sep = sep_next(sep, '\n')
			echo('function ', ent.class.lname, ':', ent.lname)
			format.method_luaarglist(ent)
			echo('\n')
			format.method_luabody(ent)
			echo('end\n')
		end
	end
	format.emit_point(t.emit_ents, 'lua_beforemetatypes')
	for i, ent in ipairs(t.type_ents) do
		if ent.type == 'class' then
			echo(ent.lname, '.typedef = \'struct{', ent.lname, '*ptr;}\'\n')
			echo(ent.lname, ':buildmetatype()\n')
		elseif ent.type == 'struct' then
			echo(ent.lname, '.typedef = \'', ent.ffiname, '\'\n')
			echo(ent.lname, ':buildmetatype()\n')
		end
	end
	format.emit_point(t.emit_ents, 'lua_end')
end
