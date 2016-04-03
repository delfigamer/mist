assert(loadfile('base.lua'))()
local parser = require('bind-parser')
local format = require('bind-format')

local filelist = {...}

-- _G.fileprefix
-- _G.compactffi
-- R_ENUM()
--   name = ...
-- R_STRUCT()
--   name = ...
-- R_CLASS()
--   name = ...
--   R_METHOD()
--     name = ...
--     stringwrap
--     addref
--     noluamethod
-- R_EMIT() ... R_END()
--   typename = fcname | lname | fcname | lname ...
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
--     lua_beforemetatypes
--     lua_end

local function getfilecontents(path)
	local file = assert(io.open(path, 'r'))
	local content = file:read('*a')
	file:close()
	return content
end

local function setfilecontents(path, content)
	local file = assert(io.open(path, 'w'))
	file:write(content)
	file:close()
end

local type_map = {
	['::methodlist_t'] = {
		type = 'special',
		fcname = '::methodlist_t',
		ffiname = 'struct methodlist_t',
	}
}
do
	local function basetype(name)
		type_map['::' .. name] = {
			type = 'base',
			fcname = name,
			ffiname = name,
		}
	end

	basetype('bool')
	basetype('int')
	basetype('int8_t')
	basetype('int16_t')
	basetype('int32_t')
	basetype('int64_t')
	basetype('uint8_t')
	basetype('uint16_t')
	basetype('uint32_t')
	basetype('uint64_t')
	basetype('size_t')
	basetype('usize_t')
	basetype('ptrdiff_t')
	basetype('intptr_t')
	basetype('uintptr_t')
	basetype('float')
	basetype('double')
	basetype('char')
end

local function type_lookup(nsprefix, cname)
	if string.sub(cname, 1, 2) == '::' then
		local typedef = type_map[cname]
		if typedef then
			return typedef
		end
	else
		local currentns = nsprefix
		while currentns do
			local fcname = currentns .. cname
			local typedef = type_map[fcname]
			if typedef then
				return typedef
			end
			currentns = string.match(currentns, '^(.*::).-::')
		end
	end
	error('unknown type: ' .. cname .. ' at ' .. nsprefix)
end

local function resolve_type(typedef)
	if typedef.type == 'void' then
	elseif typedef.type == 'reference' then
		typedef.target = type_lookup(typedef.nsprefix, typedef.cname)
	elseif typedef.type == 'pointer' then
		local base = typedef.base
		resolve_type(base)
		if base.type == 'reference' then
			local basetarget = type_lookup(base.nsprefix, base.cname)
			if basetarget.type == 'class' then
				typedef.type = 'classbox'
				typedef.target = basetarget
			end
		end
	end
end

local boundheaders = {}
local emit_ents = {}
local type_ents = {}
local method_ents = {}

local function register_entity(ent)
	if ent.type == 'emit' then
		table.append(emit_ents, ent)
		if ent.meta.typename then
			for fcname, lname in
				string.gmatch(ent.meta.typename, '([^%s|]+)%s*|%s*([%a_][%w_]*)')
			do
				type_map[fcname] = {
					type = 'special',
					fcname = fcname,
					lname = lname,
					ffiname = lname,
				}
			end
		end
	elseif ent.type == 'enum' then
		ent.lname = ent.meta.name or ent.cname
		ent.ffiname = 'int'
		type_map[ent.fcname] = ent
		table.append(type_ents, ent)
	elseif ent.type == 'struct' then
		ent.lname = ent.meta.name or ent.cname
		ent.ffiname = 'struct ' .. ent.lname
		type_map[ent.fcname] = ent
		table.append(type_ents, ent)
	elseif ent.type == 'class' then
		ent.lname = ent.meta.name or ent.cname
		ent.ffiname = ent.lname
		type_map[ent.fcname] = ent
		table.append(type_ents, ent)
	elseif ent.type == 'method' then
		ent.lname = ent.meta.name or ent.cname
		ent.flname = ent.class.lname .. '_' .. ent.lname
		table.append(method_ents, ent)
	end
end

-- load and parse the files
for i, filename in ipairs(filelist) do
	table.append(boundheaders, filename)
	local fents = parser(getfilecontents(filename))
	for i, ent in ipairs(fents) do
		register_entity(ent)
	end
end

-- resolve the references
for i, ent in ipairs(type_ents) do
	if ent.type == 'struct' then
		for j, field in ipairs(ent.fields) do
			resolve_type(field.type)
		end
	elseif ent.type == 'class' then
		if ent.parent then
			ent.parent = type_lookup(ent.nsprefix, ent.parent)
		end
	end
end
for i, ent in ipairs(method_ents) do
	if ent.type == 'method' then
		resolve_type(ent.rettype)
		for j, arg in ipairs(ent.arglist) do
			resolve_type(arg.type)
		end
	end
end

-- sort classes according to their inheritance
do
	local function preceding(ent)
		return {ent.parent}
	end
	local result, leftover = table.weak_sort(type_ents, preceding)
	if result then
		type_ents = result
	else
		error('cannot resolve class inheritance')
	end
end

local fenv = {
	boundheaders = boundheaders,
	emit_ents = emit_ents,
	type_ents = type_ents,
	method_ents = method_ents,
	fileprefix = _G.fileprefix or error('global fileprefix must be set'),
	compactffi = _G.compactffi,
}

setfilecontents(fileprefix .. '.cpp', format.format(format.cpp, fenv))
setfilecontents(fileprefix .. '.hpp', format.format(format.hpp, fenv))
setfilecontents(fileprefix .. '.lua', format.format(format.lua, fenv))
