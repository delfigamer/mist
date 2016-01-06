local modname = ...
local basearg = package.relrequire(modname, 1, 'base')
local listarg = basearg:module(modname)
local scalars = require('rs.scalars')

function listarg:init(it)
	basearg.init(self, it)
	self.items = {}
	for i, item in ipairs(it.items) do
		self.items[i] = basearg:createarg(item)
	end
end

function listarg:write(stream)
	scalars.int:write(#self.items, stream)
	for i = 1, #self.items do
		self.items[i]:writearg(stream)
	end
end

function listarg:read(stream)
	local instance = self:new()
	instance.items = {}
	local itemc = scalars.int:read(stream)
	for i = 1, itemc do
		instance.items[i] = argbase:readarg(stream)
	end
end

function listarg:defstring(lp)
	local itemstr = {}
	for i, item in ipairs(self.items) do
		itemstr[i] = item:defstring(lp)
	end
	itemstr = table.concat(itemstr, ', ')
	return string.format('[%s]', itemstr)
end
