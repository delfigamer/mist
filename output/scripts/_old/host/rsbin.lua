local modname = ...
module(modname, package.seeall)
local ffi = require('ffi')
local host = require('host')
local ml = require('host.methodlist').rsbin

function fileexists(path)
	local i = ml.fileexists(path)
	if i == 0 then
		host.checkerror()
	else
		return i == 1
	end
end

function filesize(path)
	local size = ffi.new('uint64_t[1]')
	if ml.filesize(path, size) then
		return size[0]
	else
		host.checkerror()
	end
end
