local modname = ...
local hostobject = require('host.hostobject')
local ref = hostobject:module(modname, 'ref', 'RefObject')
local ffi = require('ffi')
local ml = require('host.methodlist').utils

function ref:derive(target, modname, classname, typename)
	target = hostobject.derive(self, target, modname, classname, typename)
	return target
end

function ref:wrap(ptr)
	local obj = self:new(ptr)
	ml.refobject_addref(ptr)
	return obj
end

function ref:release()
	local rc = ml.refobject_release(self.ptr)
	self.ptr = nil
	return rc
end
