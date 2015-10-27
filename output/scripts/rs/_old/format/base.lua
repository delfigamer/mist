local modname = ...
local object = require('base.object')
local formatbase = object:module(modname)
local formattable = require('rs.formattable')
local functhreadtask = require('tasking.functhreadtask')

function formatbase:derive(target, modname, id)
	target = object.derive(self, target, modname)
	if id then
		formattable[id] = target
		target.formatid = id
	end
	return target
end

formatbase.create = nil

function formatbase:startsave(io, tag, resource, taskpriority)
	return functhreadtask:create(taskpriority, self.dosave, io, tag, resource)
end

function formatbase:startload(io, tag, resource, taskpriority)
	return functhreadtask:create(taskpriority, self.doload, io, tag, resource)
end
