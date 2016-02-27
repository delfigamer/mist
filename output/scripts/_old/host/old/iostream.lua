local modname = ...
local io = require('host.io')
local iostream = io:module(modname, 'iostream', 'IoStream', 0x3071814d)
local host = require('host')
local ffi = require('ffi')
local ml = require('host.methodlist').rsbin

local dirtable = {
	read = 1,
	write = 0,
}

function iostream:create(io, offset, direction)
	local ptr = ml.iostream_new(io.ptr, offset, dirtable[direction] or error('invalid direction'))
	if ptr ~= nil then
		return self:new(ptr)
	else
		host.checkerror()
		return nil
	end
end

