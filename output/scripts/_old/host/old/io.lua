local modname = ...
local ref = require('host.ref')
local io = ref:module(modname, 'io', 'Io', 0x658374e5)
local ffi = require('ffi')
local host = require('host')
local ml = require('host.methodlist').rsbin
local taskbase = require('host.taskbase')

function io:startread(offset, length, buffer, taskpriority)
	local result = ffi.new('int[1]')
	local taskptr = ml.io_startread(self.ptr, offset, length, buffer, result, taskpriority or 0)
	if not taskptr then
		host.checkerror()
		return nil
	end
	return taskbase:new(taskptr), result
end

function io:startwrite(offset, length, buffer, taskpriority)
	local result = ffi.new('int[1]')
	local taskptr = ml.io_startwrite(self.ptr, offset, length, buffer, result, taskpriority or 0)
	if not taskptr then
		host.checkerror()
		return nil
	end
	return taskbase:new(taskptr), result
end
