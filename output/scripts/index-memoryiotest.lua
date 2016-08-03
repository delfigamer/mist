local ffi = require('ffi')
local fileio = require('rs.fileio')
local invoke = require('base.invoke')
local hostmemoryio = require('host.memoryio')
local object = require('base.object')

local memoryio = object:derive(nil, 'memoryio')

function memoryio:init()
	self.hostmemoryio = hostmemoryio:create()
end

function memoryio:release()
	self.hostmemoryio:release()
end

function memoryio:ioread(offset, length, buffer)
	local task = self.hostmemoryio:startread(offset, length, buffer)
	while not task:isfinished() do
		coroutine.yield()
	end
	local result = task:getresult()
	if result == 0 then
		local err = task:geterror()
		if err then
			error(err)
		end
	end
	task:release()
	return result
end

function memoryio:iowrite(offset, length, buffer)
	local task = self.hostmemoryio:startwrite(offset, length, buffer)
	while not task:isfinished() do
		coroutine.yield()
	end
	local result = task:getresult()
	if result == 0 then
		local err = task:geterror()
		if err then
			error(err)
		end
	end
	task:release()
	return result
end

function memoryio:setiosize(size)
	self.hostmemoryio:setsize(size)
end

local function copyio(source, target)
	local buffer = ffi.new('uint8_t[0x10000]')
	local offset = 0ULL
	while true do
		local result = source:ioread(offset, 0x10000, buffer)
		target:iowrite(offset, result, buffer)
		offset = offset + result
		if result == 0 then
			break
		end
	end
	target:setiosize(offset)
	return offset
end

local source = fileio:create('a.txt', 'r')
local membuf = memoryio:create()
local result = invoke(copyio, source, membuf)
log(string.format('transferred %s bytes from source to memory', result))
local target = fileio:create('b.txt', 'p+')
result = invoke(copyio, membuf, target)
log(string.format('transferred %s bytes from memory to target', result))
