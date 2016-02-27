local modname = ...
local hostobject = require('host.hostobject')
local ffi = require('ffi')
local host = require('host')
local ml = require('host.methodlist').window
local windowclass = hostobject:derive(nil, modname, 'window', 'Window')

function windowclass:setshape(shape)
	if not ml.window_setshape(self.ptr, shape and shape.ptr) then
		host.checkerror()
	end
end

function windowclass:getinfo()
	local info = ml.window_getinfo(self.ptr)
	if info == nil then
		host.checkerror()
		return nil
	end
	return info
end

function windowclass:finish()
	if not ml.window_finish(self.ptr) then
		host.checkerror()
	end
end

package.modtable('host.window', windowclass:new(require('host.windowptr')))
