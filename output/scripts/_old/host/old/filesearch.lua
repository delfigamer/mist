local modname = ...
local ref = require('host.ref')
local filesearch = ref:module(modname, 'filesearch', 'FileSearch', 0x7cc7d817)
local ffi = require('ffi')
local host = require('host')
local ml = require('host.methodlist').rsbin
local types = require('host.types')

function filesearch:new(target)
	local ptr = ml.filesearch_new(target)
	if ptr ~= nil then
		return ptr
	else
		host.checkerror()
		return nil
	end
end

function filesearch:hasentry()
	local i = ml.filesearch_hasentry(self.ptr)
	if i == 0 then
		host.checkerror()
	else
		return i == 1
	end
end

function filesearch:getentry()
	local entry = types.filesearchentry()
	if not ml.filesearch_getentry(self.ptr, entry) then
		host.checkerror()
		return nil
	else
		return entry
	end
end

function filesearch:findnext()
	if not ml.filesearch_findnext(self.ptr) then
		host.checkerror()
	end
end

function filesearch:next()
	if self:hasentry() then
		local entry = self:getentry(self.entry)
		self:findnext()
		return entry
	else
		return
	end
end

function filesearch:files(target)
	local obj = self:create(target)
	obj.entry = types.filesearchentry()
	return self.next, obj
end

filesearch:buildmetatype()
