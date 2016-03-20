local modname = ...
local index = package.modtable(modname)
local clearshape = require('host.clearshape')
local ffi = require('ffi')
local info = require('host.info')
local input = require('base.input')
local window = require('host.window')

index.shape = clearshape:create()
window:setshape(ffi.cast('shape*', index.shape))

local function updatecolor(event)
	if event.arg[0] == 1 then
		local x = event.arg[1] / info.width
		local y = event.arg[2] / info.height
		index.shape:setcolor(true, ffi.new('float[4]', 0, x, y, 1))
	end
end

input.register(input.events.pointdown, updatecolor)
input.register(input.events.pointmove, updatecolor)

input.maincycle()
