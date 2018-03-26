local _G = _G

require('ffi')
require('bit')

package.path = '?.lua'

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

function require(base, uplevel, suffix)
	local modname = package.relpath(base, uplevel, suffix)
	if package.loaded[modname] then
		return package.loaded[modname]
	end
	local errstr = {}
	for i, loader in ipairs(package.loaders) do
		local lres = loader(modname)
		if type(lres) == 'string' then
			table.append(errstr, lres)
		elseif type(lres) == 'table' then
			lres.message = string.format('cannot load module %s:\n%s',
				modname, lres.message)
			error(lres, 2)
		elseif type(lres) == 'function' then
			local success, mres = pcall(lres, modname)
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
		modname, table.concat(errstr)), 2)
end

local gmeta = {
	__index = _G,
}
local gvalue = {}
local gprotect = false

function gmeta:__newindex(key, value)
	if gprotect then
		error('attempt to set global ' .. tostring(key))
	else
		rawset(_G, key, value)
	end
end

setmetatable(gvalue, gmeta)
_G._G = gvalue

function _G.protectglobaltable(protect)
	gprotect = protect
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

function _G.pcall(func, ...)
	return xpcall(func, errobj, ...)
end

function _G.log(...)
	local info = debug.getinfo(2, 'nSl')
	local str = string.format(
		'[%73s:%4i]',
		info.short_src, info.currentline)
	print(str, ...)
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

function table.makeenum(list)
	local r = {}
	for n, v in pairs(list) do
		r[n] = v
		r[v] = n
	end
	return r
end

function table.pack(...)
	return {
		count = select('#', ...),
		...
	}
end

local native_unpack = unpack

function table.unpack(t, a, b)
	local count = t.count or #t
	a = a and (a > 0 and a or count + 1 - a) or 1
	b = b and (b > 0 and b or count + 1 - b) or count
	return native_unpack(t, a, b)
end

-- sorts a list according to a weak order relation R
-- 'preceding' is a function that, given an element b, returns a list L(b)
-- let Q = {(a, b) / a is an element of L(b)}
-- R is the transitive closure of Q
-- if R is not a weak order, that is, if there are loops in Q, the function
-- returns nil, part_of_list_with_loop
-- otherwise, it returns a single list L, where R(L[i], L[j]) => i<j
-- main use: sorting interdependent entries, such that each one will only
-- depend on preceding entries in the sorted list
function table.weak_sort(list, preceding)
	local result = {}
	local ready = {}
	local current = list
	local prec_cache = {}
	while #current ~= 0 do
		local next = {}
		for i, entry in ipairs(current) do
			local prec = prec_cache[entry]
			if not prec then
				prec = preceding(entry)
				prec_cache[entry] = prec
			end
			for j, dep in ipairs(prec) do
				if not ready[dep] then
					table.append(next, entry)
					goto notready
				end
			end
			ready[entry] = true
			table.append(result, entry)
		::notready::
		end
		if #next == #current then
			return nil, next
		end
		current = next
	end
	return result
end

_G.module = nil
_G.unpack = nil
