local modname = ...
local base = require('rs.format.base')
local text = base:module(modname, 0x2f5319e1)
local exttable = require('rs.format.exttable')
local ffi = require('ffi')

exttable['txt'] = text

function text.dosave(task, io, tag, resource)
	assert(resource.istextresource)
	tag.lo = #resource.text
	tag.hi = 0
	task:yield{
		subtask = io:startwrite(0, #resource.text, resource.text, task:priority() + 1)}
end

function text.doload(task, io, tag, resource)
	assert(resource.istextresource and tag.hi == 0)
	if tag.lo == 0 then
		resource.text = ''
		return
	end
	local chars = ffi.new('char[?]', tag.lo)
	task:yield{
		subtask = io:startread(0, tag.lo, chars, task:priority() + 1)}
	resource.text = ffi.string(chars, tag.lo)
end
