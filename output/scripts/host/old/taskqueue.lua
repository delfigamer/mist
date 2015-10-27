local modname = ...
local taskqueuebase = require('host.taskqueuebase')
local taskqueue = taskqueuebase:module(modname, 'taskqueue', 'TaskQueue')
local host = require('host')
local ffi = require('ffi')
local ml = require('host.methodlist').utils

function taskqueue:create()
	local ptr = ml.taskqueue_new()
	if ptr ~= nil then
		return self:new(ptr)
	else
		host.checkerror()
		return nil
	end
end
