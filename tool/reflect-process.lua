local modname = ...
local env = require('env')
local buildconfig = require('build-config')
local parser = require('reflect-parser')
local format = require('reflect-format')
local rcache = require('reflect-cache')

-- entry point: reflect(fenv)
-- fenv fields:
--   source
--   target
--   modulename
--   suppresslua -- bool
--   suppresstypechecks -- bool

-- [[r::enum]]
--   registers an enum type
--   reflects named values
--   values are passed as numbers
-- [[r::struct]]
--   registers a POD aggregate
--   reflects entire internal structure as an FFI struct
--   values are passed as pointers to struct
-- [[r::class]]
--   registers a class type
--   reflects the type as a Lua class
--   values are passed as boxed opaque pointers
-- [[r::external]]
--   registers a class type
--   Lua reflection is not generated, unless the same type is also marked with
--     [[r::class]] somewhere in the same reflection unit
-- [[r::method]]
--   reflects a function as a pair of stubs
--   exceptions are transparently converted to Lua errors
--   object boxes are handled by the wrapper
--   may be applied to both standalone and member functions
--   [[r::hidden]] prevents the generation of the Lua wrapper
-- [[r::type]]
--   regsters a scalar type
-- [[r::field]]
--   reflects a global variable or an object field with an accessor and
--     a mutator methods
-- [[r::object]]
--   reflects a global object as an aliased box
-- for all, [[r::name("")]] sets the Lua name of the reflection
-- if r::name starts with a dot, it sets the global name of the entity
-- otherwise, the current scope is used as a module prefix
-- r_emit(<< contents >>)
--   insert 'contents' textually

local function qualifiednamestr(parts)
	local str = ''
	for i, part in ipairs(parts) do
		str = str .. '::' .. part
	end
	return str
end

local function namestr(name)
	if name.isfull then
		return qualifiednamestr(name)
	else
		return table.concat(name, '::')
	end
end

local function getsourcescope(decl)
	return qualifiednamestr(decl.scope)
end

local function getsourcefullname(decl)
	if decl.name.isfull then
		return qualifiednamestr(decl.name)
	else
		return qualifiednamestr(decl.scope) .. qualifiednamestr(decl.name)
	end
end

local function getsourcelocalname(decl)
	if #decl.name ~= 0 then
		return decl.name[#decl.name]
	else
		parser.locationerror(decl.location, 'invalid reflected entity name')
	end
end

local function getluafullname(decl)
	local fullname
	local name
	if decl.attrs['r::name'] then
		name = decl.attrs['r::name'][1]
	end
	if name then
		if string.sub(name, 1, 1) == '.' then
			fullname = string.sub(name, 2, -1)
		end
	elseif #decl.name ~= 0 then
		if decl.name.isfull then
			fullname = table.concat(decl.name, '.')
		else
			name = table.concat(decl.name, '.')
		end
	end
	if not fullname and name then
		local scope = ''
		for i, part in ipairs(decl.scope) do
			scope = scope .. part .. '.'
		end
		fullname = scope .. name
	end
	if fullname and string.match(fullname, '^[a-zA-Z0-9%_%.]+$') then
		return fullname
	else
		parser.locationerror(decl.location, 'invalid reflected entity name')
	end
end

local function getlualocalname(decl)
	local fullname
	local name
	if decl.attrs['r::name'] then
		name = decl.attrs['r::name'][1]
	end
	if not name then
		if decl.isconstructor then
			name = 'create'
		elseif #decl.name ~= 0 then
			name = decl.name[#decl.name]
		end
	end
	if name and string.match(name, '^[a-zA-Z0-9%_]+$') then
		return name
	else
		parser.locationerror(decl.location, 'invalid reflected entity name')
	end
end

local function nslookup(moduledef, scope, name)
	local namepart = qualifiednamestr(name)
	if name.isfull then
		return moduledef.namespace[namepart]
	end
	local scopepart = ''
	local result = moduledef.namespace[namepart]
	for i, part in ipairs(scope) do
		scopepart = scopepart .. '::' .. part
		local cand = moduledef.namespace[scopepart .. namepart]
		if cand then
			result = cand
		end
	end
	if result then
		return result
	else
		return nil, 'unknown name ' .. namestr(name)
	end
end

local function tryinterntype(moduledef, scope, decltype)
	if decltype.fundamental then
		return decltype
	elseif decltype.name then
		local decl, err = nslookup(moduledef, scope, decltype.name)
		if not decl then
			return nil, err
		elseif decl.fundamentaleq then
			return {
				fundamental = decl.fundamentaleq,
				sourcename = decl.sourcename,
				isconst = decltype.isconst,
				isvolatile = decltype.isvolatile}
		elseif decl.trivialname then
			return {
				trivialname = decl.trivialname,
				sourcename = decl.sourcename,
				isconst = decltype.isconst,
				isvolatile = decltype.isvolatile}
		elseif decl.classname then
			return {
				classname = decl.classname,
				classluaname = decl.luaname,
				sourcename = decl.sourcename,
				isconst = decltype.isconst,
				isvolatile = decltype.isvolatile}
		else
			return nil, namestr(decltype.name) .. ' does not name a valid type'
		end
	elseif decltype.template then
		return nil, 'NYI'
	elseif decltype.pointer then
		local target, err = tryinterntype(moduledef, scope, decltype.pointer)
		if not target then
			return nil, err
		end
		return {
			pointer = target,
			isconst = decltype.isconst,
			isvolatile = decltype.isvolatile}
	-- elseif decltype.lref then
		-- local target, err = tryinterntype(moduledef, scope, decltype.lref)
		-- if not target then
			-- return nil, err
		-- end
		-- return {
			-- lref = target,
			-- isconst = decltype.isconst,
			-- isvolatile = decltype.isvolatile}
	-- elseif decltype.rref then
		-- local target, err = tryinterntype(moduledef, scope, decltype.rref)
		-- if not target then
			-- return nil, err
		-- end
		-- return {
			-- rref = target,
			-- isconst = decltype.isconst,
			-- isvolatile = decltype.isvolatile}
	elseif decltype.array then
		local target, err = tryinterntype(moduledef, scope, decltype.array)
		if not target then
			return nil, err
		end
		return {
			array = target,
			size = decltype.size}
	else
		return nil, 'unknown type category'
	end
end

local function interntype(moduledef, decl, decltype)
	local type, err = tryinterntype(moduledef, decl.scope, decltype)
	if type then
		return type
	else
		parser.locationerror(decl.location, err)
	end
end

local function loadinclude(moduledef, decl)
	local targetname = string.match(decl.include, '(.*)%.') or decl.include
	local target = rcache.tryloadmodulebyname(targetname)
	if target then
		rcache.includemodule(moduledef, target)
	end
end

local function loadrtype(moduledef, decl)
	decl.rtype = true
	decl.sourcename = getsourcefullname(decl)
	decl.luaname = getluafullname(decl)
	decl.cname = string.gsub(decl.luaname, '%.', '_')
	decl.trivialname = decl.cname
	decl.interntype = interntype(moduledef, decl, decl.type)
	table.append(moduledef.decls, decl)
end

local function loadrstruct(moduledef, decl)
	decl.rstruct = true
	if #decl.bases ~= 0 then
		parser.locationerror(decl.location,
			'classes with bases cannot be reflected as structs')
	end
	decl.sourcename = getsourcefullname(decl)
	decl.luaname = getluafullname(decl)
	decl.cname = string.gsub(decl.luaname, '%.', '_')
	decl.trivialname = decl.cname
	decl.fields = {}
	table.append(moduledef.decls, decl)
end

local function loadrstructfield(moduledef, decl, outerdecl)
	if #decl.name ~= 1 then
		parser.locationerror(decl,
			'invalid struct field')
	end
	if not decl.isstatic then
		decl.interntype = interntype(moduledef, decl, decl.type)
		table.append(outerdecl.fields, decl)
	end
end

local function loadrclass(moduledef, decl)
	decl.rclass = true
	if #decl.bases == 1 then
		local basename = decl.bases[1]
		local basedecl, err = nslookup(moduledef, decl.scope, basename)
		if not basedecl then
			parser.locationerror(decl.location, err)
		end
		if basedecl.rclass then
			decl.base = basedecl
		else
			parser.locationerror(decl.location,
				'class base in not itself a class')
		end
	elseif #decl.bases ~= 0 then
		parser.locationerror(decl.location,
			'reflected classes cannot have more than one base')
	end
	decl.sourcename = getsourcefullname(decl)
	decl.luaname = getluafullname(decl)
	decl.cname = string.gsub(decl.luaname, '%.', '_')
	decl.classname = decl.cname
	table.append(moduledef.decls, decl)
end

local function loadrexternal(moduledef, decl)
	decl.rexternal = true
	decl.sourcename = getsourcefullname(decl)
	decl.luaname = getluafullname(decl)
	decl.cname = string.gsub(decl.luaname, '%.', '_')
	decl.classname = decl.cname
	table.append(moduledef.decls, decl)
end

local function loadrenum(moduledef, decl)
	decl.renum = true
	decl.sourcename = getsourcefullname(decl)
	decl.luaname = getluafullname(decl)
	decl.cname = string.gsub(decl.luaname, '%.', '_')
	decl.fundamentaleq = 'int'
	decl.consts = {}
	table.append(moduledef.decls, decl)
end

local function loadrenumconst(moduledef, decl, outerdecl)
	if not outerdecl.renum or #decl.name ~= 1 then
		parser.locationerror(decl,
			'invalid enum const')
	end
	local name = decl.name[1]
	if outerdecl.consts[name] then
		parser.locationerror(decl,
			'enum const is already declared')
	end
	outerdecl.consts[name] = decl.value
end

local function loadrfield(moduledef, decl, outerdecl)
	decl.rfield = true
	if outerdecl then
		if outerdecl.rclass then
			decl.outerdecl = outerdecl
			decl.sourcename = getsourcefullname(decl)
			decl.sourcelocalname = getsourcelocalname(decl)
			decl.lualocalname = getlualocalname(decl)
			decl.cname = outerdecl.cname .. '_' .. decl.lualocalname
			decl.selftype = {
				pointer = {
					classname = outerdecl.classname,
					sourcename = outerdecl.sourcename}}
		else
			parser.locationerror(decl.location,
				'r::field is in an invalid scope')
		end
	else
		decl.sourcename = getsourcefullname(decl)
		decl.luaname = getluafullname(decl)
		decl.cname = string.gsub(decl.luaname, '%.', '_')
		local nameprefix, lastname = string.match(decl.luaname, '^(.-)([^.]+)$')
		decl.accessorluaname = nameprefix .. 'get' .. lastname
		decl.mutatorluaname = nameprefix .. 'set' .. lastname
	end
	decl.interntype = interntype(moduledef, decl, decl.type)
	decl.isreadonly =
		decl.interntype.isconst or
		decl.interntype.array or
		decl.attrs['r::const'] ~= nil
	if decl.interntype.array then
		decl.interntype = {
			pointer = decl.interntype.array}
	end
	table.append(moduledef.decls, decl)
end

local function loadrobject(moduledef, decl, outerdecl)
	decl.robject = true
	if outerdecl then
		parser.locationerror(decl.location,
			'r::object must be a global object')
	end
	decl.sourcename = getsourcefullname(decl)
	decl.luaname = getluafullname(decl)
	decl.cname = string.gsub(decl.luaname, '%.', '_')
	decl.interntype = interntype(moduledef, decl, decl.type)
	if not decl.interntype.classname then
		parser.locationerror(decl.location,
			'r::object must be a variable of a boxed type')
	end
	table.append(moduledef.decls, decl)
end

local function loadrmethod(moduledef, decl, outerdecl)
	decl.rmethod = true
	decl.isnoexcept = decl.type.isnoexcept
	decl.rettype = interntype(moduledef, decl, decl.type.functionret)
	decl.argtypes = {}
	for i, arg in ipairs(decl.type.args) do
		decl.argtypes[i] = interntype(moduledef, decl, arg.type)
		if arg.attrs['r::required'] ~= nil then
			if not decl.argtypes[i].pointer then
				parser.locationerror(decl.location,
					'r::required must be applied to a pointer argument')
			end
			decl.argtypes[i].isrequired = true
		end
	end
	if outerdecl then
		if outerdecl.rclass or outerdecl.rstruct then
			decl.outerdecl = outerdecl
			if not decl.name and decl.rettype.classname == outerdecl.cname then
				decl.isconstructor = true
				decl.isstatic = true
				decl.name = {outerdecl.name[#outerdecl.name]}
				decl.rettype = {
					pointer = decl.rettype}
			end
			decl.sourcename = getsourcefullname(decl)
			decl.sourcelocalname = getsourcelocalname(decl)
			decl.lualocalname = getlualocalname(decl)
			decl.cname = outerdecl.cname .. '_' .. decl.lualocalname
			decl.selftype = {
				pointer = {
					classname = outerdecl.classname,
					sourcename = outerdecl.sourcename}}
		else
			parser.locationerror(decl.location,
				'r::method is in an invalid scope')
		end
	else
		decl.sourcename = getsourcefullname(decl)
		decl.luaname = getluafullname(decl)
		decl.cname = string.gsub(decl.luaname, '%.', '_')
	end
	table.append(moduledef.decls, decl)
end

local function loadremit(moduledef, decl)
	table.append(moduledef.decls, decl)
end

local function loaddecl(moduledef, decl)
	local outerdecl
	if decl.scope then
		outerdecl = moduledef.namespace[getsourcescope(decl)]
	end
	if decl.include then
		loadinclude(moduledef, decl)
	elseif decl.typedef then
		if decl.attrs['r::type'] ~= nil then
			loadrtype(moduledef, decl)
		end
	elseif decl.class then
		if decl.attrs['r::struct'] ~= nil then
			loadrstruct(moduledef, decl)
		elseif decl.attrs['r::class'] ~= nil then
			loadrclass(moduledef, decl)
		elseif decl.attrs['r::external'] ~= nil then
			loadrexternal(moduledef, decl)
		end
	elseif decl.enumtype or decl.enumclass then
		if decl.attrs['r::enum'] ~= nil then
			loadrenum(moduledef, decl)
		end
	elseif decl.enumconst then
		if outerdecl then
			loadrenumconst(moduledef, decl, outerdecl)
		end
	elseif decl.object then
		if decl.type.functionret then
			if decl.attrs['r::method'] ~= nil then
				loadrmethod(moduledef, decl, outerdecl)
			end
		else
			if decl.attrs['r::field'] ~= nil then
				loadrfield(moduledef, decl, outerdecl)
			elseif decl.attrs['r::object'] ~= nil then
				loadrobject(moduledef, decl, outerdecl)
			else
				if outerdecl and outerdecl.rstruct then
					loadrstructfield(moduledef, decl, outerdecl)
				end
			end
		end
	elseif decl.remit == 'r_emit' then
		loadremit(moduledef, decl)
	end
end

local function loadmodule(fenv)
	local moduledef = rcache.registermodule(fenv.modulename, fenv.source)
	local declstream = parser.parser(fenv.source)
	while true do
		local decl = declstream()
		if not decl then
			break
		end
		loaddecl(moduledef, decl)
		rcache.registerdecl(moduledef, decl)
	end
	return moduledef
end

local function rprocess(fenv)
	local moduledef = loadmodule(fenv)
	if fenv.suppresslua then
		moduledef.chunktext =
			format.tostring(format.lua, fenv, moduledef)
	else
		moduledef.chunktext =
			format.tofileandstring(fenv.target .. '.lua',
				format.lua, fenv, moduledef)
	end
	moduledef.chunkbytes =
		string.dump(assert(load(moduledef.chunktext, fenv.target .. '.lua')))
	format.tofile(fenv.target .. '.cpp',
		format.cpp, fenv, moduledef)
	format.tofile(fenv.target,
		format.r, fenv, moduledef)
end

package.modtable(modname, rprocess)
