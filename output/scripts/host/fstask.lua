local modname = ...
local ref = require('host.ref')
local fstask = ref:module(modname, 'fstask', 'FsTask')
local host = require('host')
local ffi = require('ffi')
local ml = require('host.methodlist').rsbin

function fstask:promote()
	if not ml.fstask_promote(self.ptr) then
		host.checkerror()
	end
end

function fstask:isfinished()
	local cr = ml.fstask_isfinished(self.ptr)
	if cr == 0 then
		host.checkerror()
	else
		return cr == 1
	end
end

function fstask:getresult()
	local r = ml.fstask_getresult(self.ptr)
	if r == 0 then
		host.checkerror()
	end
	return r
end

function fstask:geterror()
	local ptr = ml.fstask_geterror(self.ptr)
	if ptr == nil then
		host.checkerror()
		return nil
	end
	return ptr[0] ~= 0 and ffi.string(ptr)
end
