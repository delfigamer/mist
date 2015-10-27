local modname = ...
local monitor = require('host.monitor')
local taskqueuebase = monitor:module(modname, 'taskqueuebase', 'TaskQueueBase', 0x8da7cba4)
local host = require('host')
local ffi = require('ffi')
local ml = require('host.methodlist').utils

function taskqueuebase:insert(task)
	if not ml.taskqueuebase_insert(self.ptr, task.ptr) then
		host.checkerror()
	end
end

function taskqueuebase:iterate()
	if not ml.taskqueuebase_iterate(self.ptr) then
		host.checkerror()
	end
end
