local modname = ...
local object = require('base.object')
local queue = object:module(modname)

function queue:init()
	self.head = 1
	self.tail = 1
end

function queue:push(item)
	self[self.tail] = item
	self.tail = self.tail + 1
end

function queue:pop()
	if self.head == self.tail then
		return
	else
		local item = self[self.head]
		self.onremove(item)
		self[self.head] = nil
		self.head = self.head + 1
		return item
	end
end

function queue:peek()
	if self.head == self.tail then
		return
	else
		return self[self.head]
	end
end

function queue:clear()
	for i = self.head, self.tail - 1 do
		self.onremove(self[i])
		self[i] = nil
	end
	self.head = self.tail
end

function queue.onremove(item)
end
