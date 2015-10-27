local modname = ...
local ref = require('host.ref')
local taskbase = ref:module(modname, 'taskbase', 'TaskBase', 0x7c16c52d)
local host = require('host')
local ffi = require('ffi')
local ml = require('host.methodlist').utils

function taskbase:order()
	local order = ml.taskbase_order(self.ptr)
	if order == 0 then
		host.checkerror()
	end
	return order
end

function taskbase:priority()
	local priority = ml.taskbase_priority(self.ptr)
	if priority == 0 then
		host.checkerror()
	end
	return priority
end

function taskbase:setpriority(priority)
	if not ml.taskbase_setpriority(self.ptr, priority) then
		host.checkerror()
	end
end

function taskbase:finished()
	local finished = ml.taskbase_finished(self.ptr)
	if finished == 0 then
		host.checkerror()
		return nil
	else
		return finished == 1
	end
end

function taskbase:flush()
	if not ml.taskbase_flush(self.ptr) then
		host.checkerror()
	end
end
