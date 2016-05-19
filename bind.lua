assert(loadfile('base.lua'))()
local parser = require('bind-parser')
local format = require('bind-format')

local filelist = {...}

local structname = _G.structname or error('global structname must be set')

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
-- R_EXTERNAL()
--   typename = fcname | lname | fcname | lname ...
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
	['::' .. structname .. '_t'] = {
		type = 'special',
		fcname = '::' .. structname .. '_t',
		ffiname = 'struct ' .. structname .. '_t',
		source = '',
	}
}
do
	local function basetype(name)
		type_map['::' .. name] = {
			type = 'base',
			fcname = name,
			ffiname = name,
			source = '',
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
	return nil, 'unknown type: ' .. cname .. ' at ' .. nsprefix
end

local function resolve_type(typedef)
	if typedef.type == 'void' then
	elseif typedef.type == 'reference' then
		local target, err = type_lookup(typedef.nsprefix, typedef.cname)
		if target then
			typedef.target = target
		else
			error(typedef.source .. '\t' .. err)
		end
	elseif typedef.type == 'pointer' then
		local base = typedef.base
		resolve_type(base)
		if base.type == 'reference' then
			local basetarget, err = type_lookup(base.nsprefix, base.cname)
			if basetarget then
				if basetarget.type == 'class' or basetarget.type == 'externalclass'
				then
					typedef.type = 'classbox'
					typedef.target = basetarget
				end
			else
				error(typedef.source .. '\t' .. err)
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
				string.gmatch(ent.meta.typename,
					'%s*([^|]*[^%s|])%s*|%s*([%a_][%w_]*)')
			do
				local classfcname = string.match(fcname, '^class%s*(.*)')
				if classfcname then
					type_map[classfcname] = {
						type = 'externalclass',
						fcname = classfcname,
						lname = lname,
						ffiname = lname,
						source = ent.source,
					}
				else
					type_map[fcname] = {
						type = 'special',
						fcname = fcname,
						lname = lname,
						ffiname = lname,
						source = ent.source,
					}
				end
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
	local fents = parser(filename, getfilecontents(filename))
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
			local parent, err = type_lookup(ent.nsprefix, ent.parent)
			if parent then
				ent.parent = parent
			else
				error(ent.source .. '\t' .. err)
			end
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
		if ent.parent and ent.parent.type == 'class' then
			return {ent.parent}
		else
			return {}
		end
	end
	local result, leftover = table.weak_sort(type_ents, preceding)
	if result then
		type_ents = result
	else
		error('cannot resolve class inheritance')
		for i, ent in ipairs(leftover) do
			print('', ent.fcname .. ': ' .. ent.parent.fcname)
		end
	end
end

local fenv = {
	boundheaders = boundheaders,
	emit_ents = emit_ents,
	type_ents = type_ents,
	method_ents = method_ents,
	fileprefix = _G.fileprefix or error('global fileprefix must be set'),
	compactffi = _G.compactffi,
	structname = structname,
}

setfilecontents(fileprefix .. '.cpp', format.format(format.cpp, fenv))
setfilecontents(fileprefix .. '.hpp', format.format(format.hpp, fenv))
setfilecontents(fileprefix .. '.lua', format.format(format.lua, fenv))
