local modname = ...
local base = require('rs.format.base')
local lua = base:module(modname, 0x91167f25)
local exttable = require('rs.format.exttable')
local ffi = require('ffi')
local utility = require('base.utility')

exttable['lua'] = lua
exttable['actor'] = lua

function lua.dosave(task, io, tag, resource)
	error('cannot save a script resource')
end

function lua.doload(task, io, tag, resource)
	assert(resource.isluaresource and tag.hi == 0)
	if tag.lo == 0 then
		resource.source = ''
		resource.chunk = utility.dummy
		return
	end
	local chars = ffi.new('char[?]', tag.lo)
	task:yield{
		subtask = io:startread(0, tag.lo, chars, task:priority() + 1)}
	resource.source = ffi.string(chars, tag.lo)
	local chunk, err = loadstring(resource.source)
	if chunk then
		resource.chunk = chunk
	else
		error(err)
	end
end
