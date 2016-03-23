local modname = ...
local index = package.modtable(modname)
local clearshape = require('host.clearshape')
local ffi = require('ffi')
local info = require('host.info')
local input = require('base.input')
local primitiveshape = require('host.primitiveshape')
local shapegroup = require('host.shapegroup')
local vertexbuffer = require('host.vertexbuffer')
local window = require('host.window')

index.shape = shapegroup:create()
index.cshape = clearshape:create(true, false, false)
index.cshape:setcolor(ffi.new('float[4]', 0, 0, 0, 1))
index.pshape = primitiveshape:create(1, 1, 0, 0)
index.vbuf = vertexbuffer:create()
index.pshape:setvertexbuffer(index.vbuf)
index.cshapeentry = index.shape:insert(ffi.cast('shape*', index.cshape))
index.pshapeentry = index.shape:insert(ffi.cast('shape*', index.pshape))
window:setshape(ffi.cast('shape*', index.shape))

local function updatepos(event)
	if event.arg[0] == 1 then
		local x = event.arg[1] * (2 / info.width) - 1
		local y = event.arg[2] * (-2 / info.height) + 1
		index.pshape:setmatrix(ffi.new('float[16]',
			1, 0, 0, 0,
			0, 1, 0, 0,
			0, 0, 1, 0,
			x, y, 0, 1))
		-- index.cshape:setcolor(ffi.new('float[4]', x, y, 0, 1))
	end
end

input.register(input.events.pointdown, updatepos)
input.register(input.events.pointmove, updatepos)

input.maincycle()
