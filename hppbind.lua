assert(loadfile('base.lua'))()
setfenv(1, _G)
protectglobaltable(true)
local env = require('env')
local hppdecl = require('hppdecl')
local format = require('hppformat')

local filelist = {...}
local moduleprefix = _G.moduleprefix or ''
local targetpath = _G.targetpath or error('targetpath must be set')
local indexpath = _G.indexpath or error('indexpath must be set')
local indexname = _G.indexname or error('indexname must be set')
local suppresslua = _G.suppresslua
local suppresstypechecks = _G.suppresstypechecks
local prolog = _G.prolog or ''
local epilog = _G.epilog or ''

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

-- local function getfilecontents(path)
	-- local file = assert(io.open(env.path(path), 'r'))
	-- local content = file:read('*a')
	-- file:close()
	-- return content
-- end

-- local function setfilecontents(path, content)
	-- env.makepath(path)
	-- local file = assert(io.open(env.path(path), 'w'))
	-- file:write(content)
	-- file:close()
-- end

local namespace = {
}
do
	local function basetype(name)
		namespace['::' .. name] = {
			sourcename = name,
			trivialname = name,
			location = {{'', 1, 1}}}
	end
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
end

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
		hppdecl.locationerror(decl.location, 'invalid reflected entity name')
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
		local scope = moduleprefix
		for i, part in ipairs(decl.scope) do
			scope = scope .. part .. '.'
		end
		fullname = scope .. name
	end
	if fullname and string.match(fullname, '^[a-zA-Z0-9%_%.]+$') then
		return fullname
	else
		hppdecl.locationerror(decl.location, 'invalid reflected entity name')
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
		hppdecl.locationerror(decl.location, 'invalid reflected entity name')
	end
end

local function nslookup(scope, name)
	local namepart = qualifiednamestr(name)
	if name.isfull then
		return namespace[namepart]
	end
	local scopepart = ''
	local result = namespace[namepart]
	for i, part in ipairs(scope) do
		scopepart = scopepart .. '::' .. part
		local cand = namespace[scopepart .. namepart]
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

local function registerdecl(decl)
	local sfname = getsourcefullname(decl)
	local prev = namespace[sfname]
	if prev then
		if decl.rexternal and (prev.rclass or prev.rexternal) then
			return
		elseif prev.rexternal and decl.rclass then
			namespace[sfname] = decl
		else
			hppdecl.locationerror(decl.location,
				sfname .. ' has already been declared' ..
				hppdecl.locationstring(prev.location))
		end
	else
		namespace[sfname] = decl
	end
end

local function tryinterntype(scope, decltype)
	if decltype.fundamental then
		return decltype
	elseif decltype.name then
		local decl, err = nslookup(scope, decltype.name)
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
		local target, err = tryinterntype(scope, decltype.pointer)
		if not target then
			return nil, err
		end
		return {
			pointer = target,
			isconst = decltype.isconst,
			isvolatile = decltype.isvolatile}
	-- elseif decltype.lref then
		-- local target, err = tryinterntype(scope, decltype.lref)
		-- if not target then
			-- return nil, err
		-- end
		-- return {
			-- lref = target,
			-- isconst = decltype.isconst,
			-- isvolatile = decltype.isvolatile}
	-- elseif decltype.rref then
		-- local target, err = tryinterntype(scope, decltype.rref)
		-- if not target then
			-- return nil, err
		-- end
		-- return {
			-- rref = target,
			-- isconst = decltype.isconst,
			-- isvolatile = decltype.isvolatile}
	elseif decltype.array then
		local target, err = tryinterntype(scope, decltype.array)
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

local function interntype(decl, decltype)
	local type, err = tryinterntype(decl.scope, decltype)
	if type then
		return type
	else
		hppdecl.locationerror(decl.location, err)
	end
end

local function loadrtype(moduledef, decl)
	decl.rtype = true
	registerdecl(decl)
	decl.sourcename = getsourcefullname(decl)
	decl.luaname = getluafullname(decl)
	decl.cname = string.gsub(decl.luaname, '%.', '_')
	decl.trivialname = decl.cname
	decl.interntype = interntype(decl, decl.type)
	table.append(moduledef.decls, decl)
end

local function loadrstruct(moduledef, decl)
	decl.rstruct = true
	registerdecl(decl)
	if #decl.bases ~= 0 then
		hppdecl.locationerror(decl.location,
			'classes with bases cannot be reflected as structs')
	end
	decl.sourcename = getsourcefullname(decl)
	decl.luaname = getluafullname(decl)
	decl.cname = string.gsub(decl.luaname, '%.', '_')
	decl.trivialname = decl.cname
	decl.fields = {}
	table.append(moduledef.decls, decl)
end

local function loadrstructfield(moduledef, decl, structdecl)
	if #decl.name ~= 1 then
		hppdecl.locationerror(decl,
			'invalid struct field')
	end
	if not decl.isstatic then
		decl.interntype = interntype(decl, decl.type)
		table.append(structdecl.fields, decl)
	end
end

local function loadrclass(moduledef, decl)
	decl.rclass = true
	registerdecl(decl)
	if #decl.bases == 1 then
		local basename = decl.bases[1]
		local basedecl, err = nslookup(decl.scope, basename)
		if not basedecl then
			hppdecl.locationerror(decl.location, err)
		end
		if basedecl.rclass then
			decl.base = basedecl
		else
			hppdecl.locationerror(decl.location,
				'class base in not itself a class')
		end
	elseif #decl.bases ~= 0 then
		hppdecl.locationerror(decl.location,
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
	registerdecl(decl)
	decl.sourcename = getsourcefullname(decl)
	decl.luaname = getluafullname(decl)
	decl.cname = string.gsub(decl.luaname, '%.', '_')
	decl.classname = decl.cname
	table.append(moduledef.decls, decl)
end

local function loadrenum(moduledef, decl)
	decl.renum = true
	registerdecl(decl)
	decl.sourcename = getsourcefullname(decl)
	decl.luaname = getluafullname(decl)
	decl.cname = string.gsub(decl.luaname, '%.', '_')
	decl.fundamentaleq = 'int'
	decl.consts = {}
	table.append(moduledef.decls, decl)
end

local function loadrenumconst(moduledef, decl, enumdecl)
	if not enumdecl.renum or #decl.name ~= 1 then
		hppdecl.locationerror(decl,
			'invalid enum const')
	end
	local name = decl.name[1]
	if enumdecl.consts[name] then
		hppdecl.locationerror(decl,
			'enum const is already declared')
	end
	enumdecl.consts[name] = decl.value
end

local function loadrfield(moduledef, decl, context)
	decl.rfield = true
	if context then
		if context.rclass then
			decl.context = context
			decl.sourcename = getsourcefullname(decl)
			decl.sourcelocalname = getsourcelocalname(decl)
			decl.lualocalname = getlualocalname(decl)
			decl.cname = context.cname .. '_' .. decl.lualocalname
			decl.selftype = {
				pointer = {
					classname = context.classname,
					sourcename = context.sourcename}}
		else
			hppdecl.locationerror(decl.location,
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
	decl.interntype = interntype(decl, decl.type)
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

local function loadrobject(moduledef, decl, context)
	decl.robject = true
	if context then
		hppdecl.locationerror(decl.location,
			'r::object must be a global object')
	end
	decl.sourcename = getsourcefullname(decl)
	decl.luaname = getluafullname(decl)
	decl.cname = string.gsub(decl.luaname, '%.', '_')
	decl.interntype = interntype(decl, decl.type)
	if not decl.interntype.classname then
		hppdecl.locationerror(decl.location,
			'r::object must be a variable of a boxed type')
	end
	table.append(moduledef.decls, decl)
end

local function loadrmethod(moduledef, decl, context)
	decl.rmethod = true
	decl.isnoexcept = decl.type.isnoexcept
	decl.rettype = interntype(decl, decl.type.functionret)
	decl.argtypes = {}
	for i, arg in ipairs(decl.type.args) do
		decl.argtypes[i] = interntype(decl, arg.type)
		if arg.attrs['r::required'] ~= nil then
			if not decl.argtypes[i].pointer then
				hppdecl.locationerror(decl.location,
					'r::required must be applied to a pointer argument')
			end
			decl.argtypes[i].isrequired = true
		end
	end
	if context then
		if context.rclass or context.rstruct then
			decl.context = context
			if not decl.name and decl.rettype.classname == decl.context.cname then
				decl.isconstructor = true
				decl.isstatic = true
				decl.name = {decl.context.name[#decl.context.name]}
				decl.rettype = {
					pointer = decl.rettype}
			end
			decl.sourcename = getsourcefullname(decl)
			decl.sourcelocalname = getsourcelocalname(decl)
			decl.lualocalname = getlualocalname(decl)
			decl.cname = context.cname .. '_' .. decl.lualocalname
			decl.selftype = {
				pointer = {
					classname = context.classname,
					sourcename = context.sourcename}}
		else
			hppdecl.locationerror(decl.location,
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
	local context
	if decl.scope then
		context = namespace[getsourcescope(decl)]
	end
	if decl.typedef then
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
		if context then
			loadrenumconst(moduledef, decl, context)
		end
	elseif decl.object then
		if decl.type.functionret then
			if decl.attrs['r::method'] ~= nil then
				loadrmethod(moduledef, decl, context)
			end
		else
			if decl.attrs['r::field'] ~= nil then
				loadrfield(moduledef, decl, context)
			elseif decl.attrs['r::object'] ~= nil then
				loadrobject(moduledef, decl, context)
			else
				if context and context.rstruct then
					loadrstructfield(moduledef, decl, context)
				end
			end
		end
	elseif decl.remit == 'r_emit' then
		loadremit(moduledef, decl)
	end
end

-- load and parse the files
local pendingfiles = {}
for i, filename in ipairs(filelist) do
	pendingfiles[filename] = true
end

local function loadmodule(filename)
	local moduledef = {
		source = filename,
		decls = {}}
	moduledef.name = string.match(filename, '(.*)%.') or filename
	moduledef.identname = string.gsub(moduledef.name, '[^a-zA-Z0-9_]', '_')
	local declstream = hppdecl.parser(filename)
	while true do
		local decl = declstream()
		if not decl then
			break
		end
		if decl.include and pendingfiles[decl.include] then
			return
		end
		loaddecl(moduledef, decl)
	end
	return moduledef
end

local modulelist = {}

while #filelist ~= 0 do
	local skippedfiles = {}
	local advance = false
	for i, filename in ipairs(filelist) do
		local moduledef = loadmodule(filename)
		if moduledef then
			table.append(modulelist, moduledef)
			pendingfiles[filename] = nil
			advance = true
		else
			table.append(skippedfiles, filename)
		end
	end
	if not advance then
		for i, filename in ipairs(skippedfiles) do
			print(filename)
		end
		error('circular dependency in a reflection batch')
	end
	filelist = skippedfiles
end

local luaparts = {}
for source in string.gmatch(prolog, '[^;]+') do
	local part = {
		prolog = true,
		chunk = string.dump(assert(loadfile(env.path(source)))),
		name = source}
	table.append(luaparts, part)
end
for source in string.gmatch(epilog, '[^;]+') do
	local part = {
		epilog = true,
		chunk = string.dump(assert(loadfile(env.path(source)))),
		name = source}
	table.append(luaparts, part)
end

local indexnameparts = {}
for part in string.gmatch(indexname, '[^:]+') do
	table.append(indexnameparts, part)
end

local fenv = {
	suppresstypechecks = suppresstypechecks,
	indexpath = indexpath,
	indexnameparts = indexnameparts,
	luaparts = luaparts,
}

for i, moduledef in ipairs(modulelist) do
	if suppresslua then
		moduledef.chunktext =
			format.tostring(format.lua, fenv, moduledef)
	else
		moduledef.chunktext =
			format.tofileandstring(targetpath .. moduledef.name .. '.r.lua',
				format.lua, fenv, moduledef)
	end
	moduledef.chunkbytes =
		string.dump(assert(load(moduledef.chunktext, moduledef.name..'.r.lua')))
	format.tofile(targetpath .. moduledef.name .. '.r.cpp',
		format.cpp, fenv, moduledef)
end

-- create the index file
do
	format.tofile(indexpath .. '.hpp',
		format.indexhpp, fenv, modulelist)
	format.tofile(indexpath .. '.cpp',
		format.indexcpp, fenv, modulelist)
end
