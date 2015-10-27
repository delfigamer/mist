local modname = ...
local base = require('rs.format.base')
local png = base:module(modname, 0xf7ace538)
local defaultqueue = require('host.defaultqueue')
local exttable = require('rs.format.exttable')
local ffi = require('ffi')
local iostream = require('host.iostream')
local pngreadtask = require('host.pngreadtask')
local types = require('host.types')

exttable['png'] = png

function png.dosave(task, io, tag, resource)
	error('cannot save a texture resource')
end

function png.doload(task, io, tag, resource)
	assert(resource.texture)
	local stream = iostream:create(io, 0, 'read')
	local pngtask = pngreadtask:create(stream, task:priority() + 1)
	defaultqueue:insert(pngtask)
	task:yield{
		subtask = pngtask}
	resource.texture:assign(pngtask:getdata(), pngtask:getwidth(), pngtask:getheight())
end
