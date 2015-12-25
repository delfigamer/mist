local modname = ...
local object = require('base.object')
local stream = object:module(modname)
local token = package.relrequire(modname, 1, 'token')

function stream:init()
	self.parts = {}
	self.lastname = 0
end

function stream:genname()
	self.lastname = self.lastname + 1
	return self.lastname
end

function stream:writetoken(...)
	local t = token:create(...)
	table.append(self.parts, t)
end

function stream:substream()
	local t = self:create()
	table.append(self.parts, t)
	return t
end

function stream.instmeta:__tostring()
	local lines = {}
	for i, part in ipairs(self.parts) do
		if type(part) ~= 'table' then
			-- log(token.codemap[part.code].name)
		end
		lines[i] = tostring(part) .. '\n'
	end
	return
		'--[[stream start]]\n'
		.. table.concat(lines)
		.. '--[[stream end]]'
end
