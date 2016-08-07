local modname = ...
local object = require('base.object')
local iowrapper = object:module(modname)

iowrapper.promote = false

function iowrapper:init(hostio)
	self.hostio = hostio
end

function iowrapper:release()
	self.hostio:release()
end

function iowrapper:ioread(offset, length, buffer)
	local task = self.hostio:startread(offset, length, buffer, self.promote)
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

function iowrapper:iowrite(offset, length, buffer)
	local task = self.hostio:startwrite(offset, length, buffer, self.promote)
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

function iowrapper:setiosize(size)
	self.hostio:setsize(size)
end
