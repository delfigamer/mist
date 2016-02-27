local modname = ...
local ref = require('host.ref')
local monitor = ref:module(modname, 'monitor', 'Monitor', 0x3d897ea2)
local ffi = require('ffi')
local host = require('host')
local ml = require('host.methodlist').utils
local utility = require('base.utility')

function monitor:lock()
	if not ml.syncobject_lock(self.ptr) then
		host.checkerror()
	end
end

function monitor:unlock()
	if not ml.syncobject_unlock(self.ptr) then
		host.checkerror()
	end
end

function monitor:psynchronize(func, ...)
	self:lock()
	local ret = utility.pack(pcall(func, ...))
	self:unlock()
	return ret()
end

function monitor:synchronize(func, ...)
	self:lock()
	local ret = utility.pack(pcall(func, ...))
	self:unlock()
	if ret[1] then
		return ret(2)
	else
		error(ret[2])
	end
end
