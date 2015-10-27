local modname, windowptr, hostmethodlist = ...
module(modname, package.seeall)
local ffi = require('ffi')

ffi.cdef(io.open(_PATH .. 'scripts/host/host.h'):read('*a'))
local hostmethodlist = ffi.cast('HostMethodList*', hostmethodlist)

package.modtable('host.methodlist', hostmethodlist)
require(modname .. '.window', windowptr)

function geterror()
	local ptr = hostmethodlist.utils.cbase_geterror()
	if ptr ~= nil then
		return ffi.string(ptr)
	else
		return nil
	end
end

function seterror(msg)
	hostmethodlist.utils.cbase_seterror(msg)
	return msg
end

function checkerror()
	local err = geterror()
	if err then
		error(err)
	end
end

function write(str)
	if not hostmethodlist.utils.cbase_write(str or '') then
		checkerror()
	end
end

local getchar_buffer = ffi.new('char[5]')
function getchar()
	if not hostmethodlist.utils.cbase_getchar(getchar_buffer) then
		checkerror()
	end
	return ffi.string(getchar_buffer)
end

local function tabconcat(...)
	local args = {...}
	for i = 1, select('#', ...) do
		args[i] = tostring(args[i])
	end
	return table.concat(args, '\t')
end

function _G.print(...)
	write(tabconcat(...) .. '\n')
end

function yield()
	hostmethodlist.utils.cbase_yield()
end
