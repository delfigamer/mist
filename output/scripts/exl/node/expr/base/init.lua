local modname = ...
local node = package.relrequire(modname, 2, 'base')
local ebase = node:module(modname)
local common

function ebase:init(pr)
	node.init(self, pr)
end

function ebase:getfulltype()
	return self.fulltype
end

function ebase:getconstvalue()
	return self.constvalue
end

function ebase:gettivalue()
	local cv = self:getconstvalue()
	if cv and cv ~= self then
		return cv:gettivalue()
	end
end

common = package.relrequire(modname, 3, 'common')

local default = package.relrequire(modname, 0, 'ti')
local fulltype = package.relrequire(modname, 3, 'fulltype')

ebase.fulltype = fulltype:create(default, false, false)
