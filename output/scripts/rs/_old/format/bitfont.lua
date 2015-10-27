local modname = ...
local base = require('rs.format.base')
local bitfont = base:module(modname, 0x46d4069c)
local effont = require('extfunc.font')
local exttable = require('rs.format.exttable')
local ffi = require('ffi')

exttable['bitfont'] = bitfont

function bitfont.dosave(task, io, tag, resource)
	error('cannot save a font resource')
end

function bitfont.doload(task, io, tag, resource)
	assert(resource.isfontresource and tag.hi == 0)
	local data = ffi.new('uint8_t[?]', tag.lo)
	task:yield{
		subtask = io:startread(0, tag.lo, data, task:priority() + 1)}
	resource.bitfont = effont:create(data)
end
