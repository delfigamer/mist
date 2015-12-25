local confstr = ...

_CONFTABLE = {}
assert(loadstring(confstr, 'confstr', 't', _CONFTABLE))()

_PATH = _CONFTABLE._PATH or ''
_PLATFORM = _CONFTABLE._PLATFORM
if _PLATFORM == 'win' then
	_WIN = true
elseif _PLATFORM == 'android' then
	_ANDROID = true
end

if _WIN then
	package.dirsep = '\\'
else
	package.dirsep = '/'
end

local _G = _G

do
	local ds = package.dirsep
	local path = _PATH

	package.path =
		path .. 'scripts' .. ds .. '?.lua;' ..
		path .. 'scripts' .. ds .. '?' .. ds .. 'init.lua;' ..
		path .. 'scripts' .. ds .. '?.lb;' ..
		path .. 'scripts' .. ds .. '?' .. ds .. 'init.lb'

	package.lcpath =
		path .. '?.lc;' ..
		path .. '?.lbc'
end

function package.modtable(modname, value)
	local mt = package.loaded[modname]
	assert(not mt,
		string.format('module %s is already loaded', modname))
	mt = value or {}
	package.loaded[modname] = mt
	return mt
end

function package.relpath(base, uplevel, suffix)
	if uplevel then
		for i = 1, uplevel do
			if not base then
				error('unreachable module path requested')
			end
			base = string.match(base, '(.*)%.[^%.]-')
		end
	end
	if suffix then
		if base then
			return base .. '.' .. suffix
		else
			return suffix
		end
	else
		return base
	end
end

function module(modname, ...)
	local mt = package.modtable(modname)
	mt._NAME = modname
	mt._M = mt
	for i, func in ipairs{...} do
		func(mt)
	end
	setfenv(2, mt)
	return mt
end

function require(modname, ...)
	if package.loaded[modname] then
		return package.loaded[modname]
	end
	local errstr = ''
	for i, loader in ipairs(package.loaders) do
		local lres = loader(modname)
		if type(lres) == 'string' then
			errstr = errstr .. lres
		elseif type(lres) == 'function' then
			local success, mres = pcall(lres, modname, ...)
			if success then
				if mres then
					package.loaded[modname] = mres
					return mres
				else
					if package.loaded[modname] then
						return package.loaded[modname]
					else
						package.loaded[modname] = true
						return true
					end
				end
			else
				package.loaded[modname] = nil
				mres.message = string.format('failed to load %s:\n%s',
					modname, mres.message)
				error(mres, 2)
			end
		end
	end
	error(string.format('cannot load module %s:\n%s',
		modname, errstr), 2)
end

function package.relrequire(base, uplevel, suffix, ...)
	local modname = package.relpath(base, uplevel, suffix)
	return require(modname, ...)
end

package.loaders[#package.loaders + 1] = function(modname)
	local path, perr = package.searchpath(modname, package.lcpath)
	if not path then
		return perr
	end
	local env = {}
	local f, err = loadfile(path, 'bt', env)
	if f then
		return function(modname, ienv)
			setmetatable(env, {__index = ienv})
			f(modname)
			return env
		end
	else
		return '\n' .. err
	end
end

function package.modulepack(pt)
	local target = pt.target or {}
	local prefix = pt.prefix or ''
	for i, name in ipairs(pt.names) do
		target[name] = require(prefix .. name)
	end
	return target
end

local gmeta = {}

local gprotect = false

function gmeta:__newindex(key, value)
	if gprotect then
		local kstr
		if type(key) == 'string' then
			kstr = string.format('%q', key)
		else
			kstr = tostring(key)
		end
		local info = debug.getinfo(2, 'nSl')
		local str = string.format(
			'[%48s:%24s@%4i]\t_G[%s] = %s',
			info.short_src, info.name or '', info.currentline, kstr, value)
		print(str)
	end
	rawset(_G, key, value)
end

setmetatable(_G, gmeta)

function protectglobaltable(protect)
	gprotect = protect
end

function assert(value, errstr, pos)
	return value or error(
		errstr or 'assertion failure', pos and (pos == 0 and 0 or pos+1) or 2)
end

errobj_meta = {}

errobj_meta.__exception = true

function errobj_meta:__tostring()
	return self.message .. self.traceback
end

local function errobj(msg)
	local mt = getmetatable(msg)
	if mt and mt.__exception then
		return msg
	end
	local t = {
		message = tostring(msg),
		traceback = debug.traceback('', 2)}
	return setmetatable(t, errobj_meta)
end

function pcall(func, ...)
	return xpcall(func, errobj, ...)
end

function log(...)
	local info = debug.getinfo(2, 'nSl')
	local str = string.format(
		'[%48s:%24s@%4i]',
		info.short_src, info.name or '', info.currentline)
	print(str, ...)
end

function string.split(text, delim, plain)
	local result = {}
	local i = 0
	local spos = 1
	repeat
		i = i + 1
		local dspos, depos = text:find(delim, spos, plain)
		if not dspos then
			break
		end
		result[i] = text:sub(spos, dspos - 1)
		spos = depos + 1
	until false
	if spos <= #text then
		result[i] = text:sub(spos)
	end
	return result
end

function table.spairs_next(list, i)
	i = i + 1
	if i <= #list then
		return i, list[i].key, list[i].value
	end
end

function table.spairs_comparer(a, b)
	return a.key < b.key
end

function table.spairs(target, comparer)
	local list = {}
	local i = 0
	for key, value in pairs(target) do
		i = i + 1
		list[i] = {key = key, value = value}
	end
	table.sort(list, comparer or table.spairs_comparer)
	return table.spairs_next, list, 0
end

function table.deepget(t, k, k2, ...)
	if k2 then
		if t[k] ~= nil then
			return table.deepget(t[k], k2, ...)
		else
			return nil
		end
	else
		return t[k]
	end
end

function table.deepset(t, nv, k, k2, ...)
	if k2 then
		if t[k] == nil then
			t[k] = {}
		end
		return table.deepset(t[k], nv, k2, ...)
	else
		t[k] = nv
	end
end

function table.provide(t, k)
	local st = t[k]
	if not st then
		st = {}
		t[k] = st
	end
	return st
end

function table.append(t, v)
	local i = #t + 1
	t[i] = v
	return i
end

function table.pop(t)
	local i = #t
	local v = t[i]
	t[i] = nil
	return v
end

function table.makeset(list)
	local r = {}
	for i, v in ipairs(list) do
		r[v] = true
	end
	return r
end
