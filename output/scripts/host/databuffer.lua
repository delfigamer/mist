local modname = ...
local ref = require('host.ref')
local databuffer = ref:module(modname, 'databuffer', 'DataBuffer')
local ffi = require('ffi')
local host = require('host')
local ml = require('host.methodlist').utils
local types = require('host.types')

function databuffer:create(length, capacity, data)
	local ptr = ml.databuffer_new(length or 0, capacity or 0, data or nil)
	if ptr ~= nil then
		return self:new(ptr)
	else
		host.checkerror()
		return nil
	end
end

function databuffer:getdata()
	local value = ml.databuffer_getdata(self.ptr)
	if value == nil then
		host.checkerror()
		return nil
	end
	return value
end

function databuffer:getlength()
	local value = ml.databuffer_getlength(self.ptr)
	if value == 0 then
		host.checkerror()
	end
	return value
end

function databuffer:setlength(length)
	if not ml.databuffer_setlength(self.ptr, length) then
		host.checkerror()
	end
end

function databuffer:getcapacity()
	local value = ml.databuffer_getcapacity(self.ptr)
	if value == 0 then
		host.checkerror()
	end
	return value
end

function databuffer:defstring(datamaxlen)
	datamaxlen = datamaxlen or 256
	if self.ptr == nil then
		return 'databuffer< nil >'
	end
	local length = self:getlength()
	local capacity = self:getcapacity()
	local data = self:getdata()
	local datastr
	if length <= datamaxlen then
		datastr = string.format('%q', ffi.string(data, length))
	else
		datastr = string.format('%q...', ffi.string(data, datamaxlen))
	end
	return string.format('databuffer< %#x, %#x, %s >', length, capacity, datastr)
end

function databuffer.instmeta:__tostring()
	return self:defstring(64)
end
