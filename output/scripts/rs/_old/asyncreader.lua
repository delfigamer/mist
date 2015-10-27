local modname = ...
local object = require('base.object')
local asyncreader = object:module(modname)
local defer = require('base.defer')
local ffi = require('ffi')

function asyncreader:init(stream, reader, ...)
	self.stream = stream
	self.buffer = {}
	self.reader = coroutine.create(reader)
	coroutine.resume(self.reader, self, ...)
	defer.push(self.iterate, self)
end

function asyncreader:halt()
	self.reader = nil
end

function asyncreader:isfinished()
	return coroutine.status(self.reader) ~= 'suspended'
end

function asyncreader:iterate(...)
	if self.reader and coroutine.status(self.reader) == 'suspended' then
		local suc, ret = coroutine.resume(self.reader, ...)
		if not suc then
			print(ret)
		end
		if coroutine.status(self.reader) == 'suspended' then
			return false
		else
			return true
		end
	else
		return true
	end
end

function asyncreader:peek()
	if not self.buffer[1] and not self.beof then
		local cbuf = ffi.new('uint8_t[256]')
		local len = self.stream:threadread(256, cbuf)
		for i = 0, len-1 do
			self.buffer[len-i] = string.char(cbuf[i])
		end
		if len < 256 then
			self.beof = true
		end
	end
	return self.buffer[#self.buffer]
end

function asyncreader:get()
	self:peek()
	return table.pop(self.buffer)
end

function asyncreader:unget(ch)
	table.append(self.buffer, ch)
end

function asyncreader:read(length, buffer)
	buffer = ffi.cast('uint8_t*', buffer)
	local len = #self.buffer
	if len >= length then
		for i = len, len - length + 1, -1 do
			buffer[len - i] = string.byte(self.buffer[i])
			self.buffer[i] = nil
		end
		return len
	else
		for i = len, 1, -1 do
			buffer[len - i] = string.byte(self.buffer[i])
			self.buffer[i] = nil
		end
		return len + self.stream:threadread(length - len, buffer + len)
	end
end
