local modname = ...
local object = require('base.object')
local teststream = object:module(modname)
local dword = require('host.types').dword
local ffi = require('ffi')
local host = require('host')
local uint64 = require('rs.scalars').uint64
local iostream = require('rs.iostream')
local memoryio = require('rs.memoryio')

local tswsubstream = object:derive(nil, modname .. ':substream')

function tswsubstream:init(parent)
	self.parent = parent
	self.memoryio = memoryio:create()
	self.stream = iostream:create(self.memoryio)
end

function tswsubstream:read(length, data)
	return self.stream:read(length, data)
end

function tswsubstream:write(length, data)
	return self.stream:write(length, data)
end

function tswsubstream:skip(length)
	return self.stream:skip(length)
end

function tswsubstream:readsubstream()
	return self.stream:readsubstream()
end

function tswsubstream:writesubstream()
	return self.stream:writesubstream()
end

function tswsubstream:close()
	uint64:write(self.memoryio.size, self.parent)
	local sized = dword(self.memoryio.size)
	for pageindex = 0, sized.h - 1 do
		local page = self.memoryio.pages[pageindex] or {}
		for bufindex = 0, 0xffff do
			local buffer = page[bufindex]
			if buffer then
				self.parent:write(0x10000, buffer)
			else
				self.parent:skip(0x10000)
			end
		end
	end
	local page = self.memoryio.pages[sized.h] or {}
	for bufindex = 0, bit.rshift(sized.l, 16) - 1 do
		local buffer = page[bufindex]
		if buffer then
			self.parent:write(0x10000, buffer)
		else
			self.parent:skip(0x10000)
		end
	end
	local buffer = page[bit.rshift(sized.l, 16)]
	if buffer then
		self.parent:write(bit.band(sized.l, 0xffff), buffer)
	else
		self.parent:skip(bit.band(sized.l, 0xffff))
	end
	if self.parent.position8 ~= 0 then
		self.parent:skip(8 - self.parent.position8)
	end
end

teststream.position8 = 0

function teststream:write(length, buffer)
	local ptr = ffi.cast('uint8_t const*', buffer)
	for i = 0, length-1 do
		host.write(string.format('[%.2x]', ptr[i]))
	end
	self.position8 = bit.band(self.position8 + length, 7)
	return length
end

function teststream:skip(length)
	for i = 1, length do
		host.write('[--]')
	end
	self.position8 = bit.band(self.position8 + length, 7)
	return length
end

function teststream:writesubstream()
	if self.position8 ~= 0 then
		self:skip(8 - self.position8)
	end
	return tswsubstream:create(self)
end
