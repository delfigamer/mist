local modname = ...
local object = require('base.object')
local set = object:module(modname)

function set:init()
	self.count = 0
	self.map = {}
end

function set:insert(item)
	if self.map[item] then
		return
	end
	self.count = self.count + 1
	self[self.count] = item
	self.map[item] = self.count
end

function set:remove(item)
	local id = self.map[item]
	if not id then
		return
	end
	self.onremove(item)
	if self.count == id then
		self[id] = nil
	else
		local lastitem = self[self.count]
		self[id] = lastitem
		self.map[lastitem] = id
		self[self.count] = nil
	end
	self.map[item] = nil
	self.count = self.count - 1
end

function set:clear()
	for i, item in ipairs(self) do
		self.onremove(item)
		self[i] = nil
		self.map[item] = nil
	end
	self.count = 0
end

function set:pairs()
	return ipairs(self)
end

function set.onremove(item)
end
