local modname = ...
local host = package.modtable(modname)
local ffi = require('ffi')

ffi.cdef(io.open(_PATH .. 'scripts/host/host.h'):read('*a'))
local hostmethodlist = ffi.cast('HostMethodList*',
	require('host.methodlistptr'))

package.modtable('host.methodlist', hostmethodlist)
require(modname .. '.window')

function host.geterror()
	local ptr = hostmethodlist.utils.cbase_geterror()
	if ptr ~= nil then
		return ffi.string(ptr)
	else
		return nil
	end
end

function host.seterror(msg)
	hostmethodlist.utils.cbase_seterror(msg)
	return msg
end

function host.checkerror()
	local err = host.geterror()
	if err then
		error(err)
	end
end

function host.write(str)
	if not hostmethodlist.utils.cbase_write(str or '') then
		host.checkerror()
	end
end

local getchar_buffer = ffi.new('char[5]')
function host.getchar()
	if not hostmethodlist.utils.cbase_getchar(getchar_buffer) then
		host.checkerror()
	end
	return ffi.string(getchar_buffer)
end

function host.yield()
	hostmethodlist.utils.cbase_yield()
end

local function tabconcat(...)
	local args = {...}
	for i = 1, select('#', ...) do
		args[i] = tostring(args[i])
	end
	return table.concat(args, '\t')
end

function _G.print(...)
	host.write(tabconcat(...) .. '\n')
end
