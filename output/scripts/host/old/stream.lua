local modname = ...
local ref = require('host.ref')
local stream = ref:module(modname, 'stream', 'Stream', 0x8bea74b5)
local ffi = require('ffi')
local host = require('host')
local ml = require('host.methodlist').utils
local taskbase = require('host.taskbase')

function stream:starttransfer(length, buffer, taskpriority)
	local result = ffi.new('int[1]')
	local taskptr = ml.stream_starttransfer(self.ptr, length, buffer, result, taskpriority or 0)
	if not taskptr then
		host.checkerror()
		return nil
	end
	return taskbase:wrapnew(taskptr), result
end
