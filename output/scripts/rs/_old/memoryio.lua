local modname = ...
local object = require('base.object')
local memoryio = object:module(modname)
local dword = require('host.types').dword
local ffi = require('ffi')
local host = require('host')

function memoryio:init()
	self.pages = {}
	self.size = 0ULL
end

function memoryio:iowrite(offset, length, data)
	offset = offset + 0ULL
	local result = 0
	local offsetd = dword(offset)
	data = ffi.cast('uint8_t const*', data)
	while length > 0 do
		local pageindex = offsetd.h
		local page = self.pages[pageindex]
		if not page then
			page = {}
			self.pages[pageindex] = page
		end
		local bufindex = bit.rshift(offsetd.l, 16)
		local bufpos = bit.band(offsetd.l, 0xffff)
		local buffer = page[bufindex]
		if not buffer then
			buffer = ffi.new('uint8_t[0x10000]')
			page[bufindex] = buffer
		end
		local advance = 0x10000 - bufpos
		if advance > length then
			advance = length
		end
		ffi.copy(buffer + bufpos, data, advance)
		length = length - advance
		data = data + advance
		offset = offset + advance
		result = result + advance
		offsetd.d = offset
	end
	if self.size < offset then
		self.size = offset
	end
	return result
end

function memoryio:ioread(offset, length, data)
	offset = offset + 0ULL
	if offset + length > self.size then
		if offset >= self.size then
			return 0
		else
			length = self.size - offset
			if length > 0x7fffffffULL then
				return 0
			else
				length = tonumber(length)
			end
		end
	end
	local result = 0
	local offsetd = dword(offset)
	data = ffi.cast('uint8_t*', data)
	while length > 0 do
		local pageindex = offsetd.h
		local bufindex = bit.rshift(offsetd.l, 16)
		local bufpos = bit.band(offsetd.l, 0xffff)
		local advance = 0x10000 - bufpos
		if advance > length then
			advance = length
		end
		local page = self.pages[pageindex]
		if page then
			local buffer = page[bufindex]
			if buffer then
				ffi.copy(data, buffer + bufpos, advance)
			end
		end
		length = length - advance
		data = data + advance
		offset = offset + advance
		result = result + advance
		offsetd.d = offset
	end
	return result
end

function memoryio:setiosize(size)
	local oldsized = dword(self.size)
	local sized = dword(size)
	if sized.h < oldsized.h then
		for i = sized.h+1, oldsized.h do
			self.pages[i] = nil
		end
		local page = self.pages[self.sized.h]
		for i = bit.rshift(sized.l)+1, 0xffff do
			page[i] = nil
		end
	elseif sized.h == oldsized.h then
		local page = self.pages[self.sized.h]
		for i = bit.rshift(sized.l)+1, bit.rshift(oldsized.l) do
			page[i] = nil
		end
	end
end
