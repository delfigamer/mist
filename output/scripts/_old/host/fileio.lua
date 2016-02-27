local modname = ...
local ref = require('host.ref')
local fileio = ref:module(modname, 'fileio', 'FileIo')
local host = require('host')
local ffi = require('ffi')
local fstask = require('host.fstask')
local ml = require('host.methodlist').rsbin

local modetable = {
	['r'] = 0,
	['r+'] = 1,
	['w'] = 2,
	['w+'] = 3,
	['p+'] = 4,
}

function fileio:create(path, mode)
	local ptr = ml.fileio_new(path, modetable[mode] or mode)
	if ptr ~= nil then
		return self:new(ptr)
	else
		host.checkerror()
		return nil
	end
end

function fileio:startread(offset, length, buffer)
	local taskptr = ml.fileio_startread(self.ptr, offset, length, buffer)
	if not taskptr then
		host.checkerror()
		return nil
	end
	return fstask:new(taskptr)
end

function fileio:startwrite(offset, length, buffer)
	local taskptr = ml.fileio_startwrite(self.ptr, offset, length, buffer)
	if not taskptr then
		host.checkerror()
		return nil
	end
	return fstask:new(taskptr)
end

function fileio:setsize(size)
	if not ml.fileio_setsize(self.ptr, size) then
		host.checkerror()
	end
end
