local modname = ...
local base = require('rs.format.base')
local raw = base:module(modname)
local ffi = require('ffi')

function raw:dosave(io, tag, resource)
	io:threadwrite(0, #resource.data, resource.data)
end

function raw:doload(io, tag, resource)
end
