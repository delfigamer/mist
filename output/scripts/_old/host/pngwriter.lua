local modname = ...
local hostobject = require('host.hostobject')
local pngwriter = hostobject:module(modname, 'pngwriter', 'PngWriter')
local ffi = require('ffi')
local host = require('host')
local ml = require('host.methodlist').rsbin

function pngwriter:create(format, width, height, data)
	local ptr = ml.pngwriter_new(format, width, height, data and data.ptr or data)
	if ptr ~= nil then
		return self:new(ptr)
	else
		host.checkerror()
		return nil
	end
end

function pngwriter:release()
	if self.ptr ~= nil then
		ml.pngwriter_delete(self.ptr)
		self.ptr = nil
	end
end

function pngwriter:grab(length, buffer)
	local result = ffi.new('int[1]')
	if not ml.pngwriter_grab(self.ptr, length, buffer, result) then
		host.checkerror()
		return
	end
	return result[0]
end

function pngwriter:isfinished()
	local cr = ml.pngwriter_isfinished(self.ptr)
	if cr == 0 then
		host.checkerror()
	else
		return cr == 1
	end
end
