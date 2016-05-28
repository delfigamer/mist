local modname = ...
local index = package.modtable(modname)
-- local clearshape = require('host.clearshape')
local databuffer = require('host.databuffer')
local ffi = require('ffi')
local info = require('host.info')
local input = require('base.input')
-- local primitiveshape = require('host.primitiveshape')
-- local shapegroup = require('host.shapegroup')
-- local staticvertexbuffer = require('host.staticvertexbuffer')
-- local vertexdeclaration = require('host.vertexdeclaration')
local window = require('host.window')

-- ffi.cdef[[
	-- typedef struct
	-- {
		-- float pos[ 2 ];
		-- uint8_t color[ 4 ];
	-- } Vertex;
-- ]]

-- do
	-- local vdata = databuffer:create(
		-- ffi.sizeof('Vertex') * 6, ffi.sizeof('Vertex') * 7, nil)
	-- local vertices = ffi.cast('Vertex*', vdata:getdata())
	-- vertices[0] = {{-0.5, -0.5}, {0xff,    0,    0, 0xff}}
	-- vertices[1] = {{-0.5,  0.5}, {0xff, 0xff,    0, 0xff}}
	-- vertices[2] = {{ 0.0, -0.5}, {   0, 0xff,    0, 0xff}}
	-- vertices[3] = {{ 0.0,  0.5}, {   0, 0xff, 0xff, 0xff}}
	-- vertices[4] = {{ 0.5, -0.5}, {   0,    0, 0xff, 0xff}}
	-- vertices[5] = {{ 0.5,  0.5}, {0xff,    0, 0xff, 0xff}}
	-- vertices[6] = {{-0.5, -0.5}, {0xff,    0,    0, 0xff}}

	-- local vdecldata = databuffer:create(
		-- ffi.sizeof('struct vertexdeclelement') * 2,
		-- ffi.sizeof('struct vertexdeclelement') * 2,
		-- nil)
	-- local vdeclelems = ffi.cast('struct vertexdeclelement*', vdecldata:getdata())
	-- vdeclelems[0] = {
		-- attribute = 0,
		-- offset = ffi.offsetof('Vertex', 'pos'),
		-- format = vertexdeclaration.format.float2,
	-- }
	-- vdeclelems[1] = {
		-- attribute = 1,
		-- offset = ffi.offsetof('Vertex', 'color'),
		-- format = vertexdeclaration.format.ubyte4n,
	-- }
	-- local vdecl = vertexdeclaration:create(vdecldata, ffi.sizeof('Vertex'))

	-- index.shape = shapegroup:create()
	-- index.cshape = clearshape:create(true, false, false)
	-- index.cshape:setcolor(ffi.new('float[4]', 0, 0, 0, 1))
	-- index.pshape = primitiveshape:create(3, 1, 0, 0)
	-- index.vbuf = staticvertexbuffer:create(vdecl)
	-- index.vbuf:assign(vdata)
	-- index.pshape:setvertexbuffer(index.vbuf)
	-- index.cshapeentry = index.shape:insert(index.cshape)
	-- index.pshapeentry = index.shape:insert(index.pshape)
	-- window:setshape(index.shape)
-- end
collectgarbage()

local function updatepos(event)
	if event.a == 1 then
		-- local x = event.b * (2 / info.width) - 1
		-- local y = event.c * (-2 / info.height) + 1
		-- index.pshape:setmatrix(ffi.new('float[16]',
			-- 1, 0, 0, 0,
			-- 0, 1, 0, 0,
			-- 0, 0, 1, 0,
			-- x, y, 0, 1))
		-- index.cshape:setcolor(ffi.new('float[4]', x, y, 0, 1))
	end
end

input.register(input.events.pointdown, updatepos)
input.register(input.events.pointmove, updatepos)

input.maincycle()
