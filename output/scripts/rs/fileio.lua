local modname = ...
local object = require('base.object')
local fileio = object:module(modname)
local hostfileio = require('host.fileio')

function fileio:init(path, mode)
	self.hostfileio = hostfileio:create(path, mode)
end

function fileio:release()
	self.hostfileio:release()
end

function fileio:ioread(offset, length, buffer)
	local task = self.hostfileio:startread(offset, length, buffer)
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

function fileio:iowrite(offset, length, buffer)
	local task = self.hostfileio:startwrite(offset, length, buffer)
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

function fileio:setiosize(size)
	self.hostfileio:setsize(size)
end
