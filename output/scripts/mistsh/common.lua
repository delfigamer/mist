local modname = ...
module(modname, package.seeall)
local default

function createnode(pr, ...)
	local success, result = pcall(require, 'mistsh.node.' .. pr.name)
	if success then
		return result:createnode(pr, ...)
	else
		print(result)
		return default:createnode(pr, ...)
	end
end

function listdefstring(list, lp)
	local lines = {}
	for i, stat in ipairs(list) do
		lines[i] = '\n' .. lp .. stat:defstring(lp)
	end
	return table.concat(lines)
end

function optdefstring(self, def, lp)
	if self then
		return self:defstring(lp)
	else
		return def
	end
end

function listbuildstring(list, ...)
	local lines = {}
	for i, stat in ipairs(list) do
		lines[i] = '\n' .. stat:build(...)
	end
	return table.concat(lines)
end

function optbuildstring(self, def)
	if self then
		return self:build()
	else
		return def
	end
end

if _PLATFORM == 'win' then
-- Shader model 3.0 (Direct3D 9.0c)
	target = 'hlsl_3_0'
elseif _PLATFORM == 'android' then
-- GLSL 1.00 (GL ES 2.0)
	target = 'glsl_100'
end

default = require('mistsh.node.default')

