local modname = ...
local object = require('base.object')
local exception = object:module(modname)

function exception:init(message, depth)
	self.message = tostring(message)
	self.traceback = debug.traceback('', depth and depth+2 or 3)
end

exception.instmeta.__exception = true

function exception.instmeta:__tostring()
	return self.message .. self.traceback
end

function _G.errobj(msg)
	local mt = getmetatable(msg)
	if mt and mt.__exception then
		return msg
	end
	return exception:create(msg)
end
