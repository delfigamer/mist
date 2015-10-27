local modname = ...
local base = require('rs.format.base')
local metadata = base:module(modname)
local ffi = require('ffi')
local serialize = require('base.serialize')

function metadata.dosave(task, io, tag, resource)
	if not next(resource.metadata) then
		tag.hi, tag.lo = 0, 0
		return
	end
	local text = serialize.serialize(resource.metadata)
	tag.lo = #text
	tag.hi = 0
	task:yield{
		subtask = io:startwrite(0, #text, text, task:priority() + 1)}
end

function metadata.doload(task, io, tag, resource)
	assert(tag.hi == 0)
	if tag.lo == 0 then
		resource.metadata = {}
		return
	end
	local chars = ffi.new('char[?]', tag.lo)
	task:yield{
		subtask = io:startread(0, tag.lo, chars, task:priority() + 1)}
	local text = ffi.string(chars, tag.lo)
	resource.metadata = serialize.deserialize(text)
end
