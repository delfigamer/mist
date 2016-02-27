local modname = ...
local hostobject = require('host.hostobject')
local pngreader = hostobject:module(modname, 'pngreader', 'PngReader')
local databuffer = require('host.databuffer')
local host = require('host')
local ml = require('host.methodlist').rsbin

function pngreader:create(format)
	local ptr = ml.pngreader_new(format)
	if ptr ~= nil then
		return self:new(ptr)
	else
		host.checkerror()
		return nil
	end
end

function pngreader:release()
	if self.ptr ~= nil then
		ml.pngreader_delete(self.ptr)
		self.ptr = nil
	end
end

function pngreader:feed(length, buffer)
	if not ml.pngreader_feed(self.ptr, length, buffer) then
		host.checkerror()
	end
end

function pngreader:isfinished()
	local cr = ml.pngreader_isfinished(self.ptr)
	if cr == 0 then
		host.checkerror()
	else
		return cr == 1
	end
end

function pngreader:getwidth()
	local width = ml.pngreader_getwidth(self.ptr)
	if width == 0 then
		host.checkerror()
	end
	return width
end

function pngreader:getheight()
	local height = ml.pngreader_getheight(self.ptr)
	if height == 0 then
		host.checkerror()
	end
	return height
end

function pngreader:getdata()
	local data = ml.pngreader_getdata(self.ptr)
	if data == nil then
		host.checkerror()
		return nil
	else
		return databuffer:wrap(data)
	end
end
