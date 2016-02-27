local modname = 'base.object'
local object = package.modtable(modname)
local ffi = require('ffi')

object.instmeta = {
	__index = object}
object._NAME = modname
object['#' .. modname] = true

function object:copy(target)
	target = target or {}
	for k,v in pairs(self) do
		target[k] = v
	end
	return target
end

function object:derive(target, modname)
	assert(modname)
	local new = object.copy(self, target)
	new.instmeta = object.copy(self.instmeta)
	new.instmeta.__index = new
	new._NAME = modname
	new['#' .. modname] = true
	return new
end

function object:module(modname, ...)
	mt = self:derive(nil, modname, ...)
	package.modtable(modname, mt)
	return mt
end

function object:new()
	local new = {}
	return setmetatable(new, self.instmeta)
end

function object:create(...)
	local new = self:new()
	new:init(...)
	return new
end

function object:init()
end

function object:release()
end

local function prettyvalue(val)
	if type(val) == 'string' then
		return string.format('%q', val)
	else
		return tostring(val)
	end
end

local function comparer(a, b)
	return a[1] < b[1]
end

function object:defstring()
	local lines = {}
	local i = 1
	for k, v in pairs(self) do
		lines[i] = {prettyvalue(k), prettyvalue(v)}
		i = i + 1
	end
	table.sort(lines, comparer)
	for i, v in ipairs(lines) do
		lines[i] = string.format('\t[%s] = %s,\n',
			v[1], v[2])
	end
	return (self._NAME or '') .. '{\n' .. table.concat(lines) .. '}'
end
