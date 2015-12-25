local modname = ...
local object = require('base.object')
local exception = object:module(modname)

function exception:create()
	error('cannot create an exception, use exception:throw instead')
end

function exception:throw(...)
	local inst = self:new()
	inst.traceback = debug.traceback('', 2)
	inst:init(...)
	error(inst)
end

function exception:init(message)
	self.message = tostring(message)
end

exception.instmeta.__exception = true

function exception:tostring()
	return self.message .. self.traceback
end

function exception.instmeta:__tostring()
	return self:tostring()
end
