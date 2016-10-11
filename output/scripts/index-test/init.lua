local modname = ...
local index = package.modtable(modname)
local clearshape = require('host.clearshape')
local databuffer = require('host.databuffer')
local ffi = require('ffi')
local info = require('host.info')
local input = require('base.input')
local primitiveshape = require('host.primitiveshape')
local program = require('host.program')
local pt = require('host.programtoken')
local shapegroup = require('host.shapegroup')
local staticvertexbuffer = require('host.staticvertexbuffer')
local vertexdeclaration = require('host.vertexdeclaration')
local window = require('host.window')

local rsutil = require('rs.util')
local dbptype = require('rs.dbptype')

ffi.cdef[[
	typedef struct
	{
		float pos[ 2 ];
		uint8_t color[ 4 ];
	} Vertex;
]]

local floatint_t = ffi.typeof[[
	union {
		float f;
		int i;
	}
]]

local function ftoi(n)
	return floatint_t(n).i
end

do
	local vdata = databuffer:create(
		ffi.sizeof('Vertex') * 6, ffi.sizeof('Vertex') * 7, nil)
	local vertices = ffi.cast('Vertex*', vdata:getdata())
	vertices[0] = {{-0.5,  0.5}, {0xff,    0,    0, 0xff}}
	vertices[1] = {{-0.5, -0.5}, {0xff, 0xff,    0, 0xff}}
	vertices[2] = {{ 0.0,  0.5}, {   0, 0xff,    0, 0xff}}
	vertices[3] = {{ 0.0, -0.5}, {   0, 0xff, 0xff, 0xff}}
	vertices[4] = {{ 0.5,  0.5}, {   0,    0, 0xff, 0xff}}
	vertices[5] = {{ 0.5, -0.5}, {0xff,    0, 0xff, 0xff}}
	vertices[6] = {{-0.5,  0.5}, {0xff,    0,    0, 0xff}}

	local vdecldata = databuffer:create(
		ffi.sizeof('struct vertexdeclelement') * 2,
		ffi.sizeof('struct vertexdeclelement') * 2,
		nil)
	local vdeclelems = ffi.cast('struct vertexdeclelement*', vdecldata:getdata())
	vdeclelems[0] = {
		attribute = 0,
		offset = ffi.offsetof('Vertex', 'pos'),
		format = vertexdeclaration.format.float2,
	}
	vdeclelems[1] = {
		attribute = 1,
		offset = ffi.offsetof('Vertex', 'color'),
		format = vertexdeclaration.format.ubyte4n,
	}
	local vdecl = vertexdeclaration:create(vdecldata, ffi.sizeof('Vertex'))

	do
		local programsource = {
					pt.attribute_1,
					pt.literal_float1,
						ftoi(1),
				pt.multiply,
				pt.literal_float4,
					ftoi(1/2.2), ftoi(1/2.2), ftoi(1/2.2), ftoi(1),
			pt.power,
		}
		local db = databuffer:create(#programsource*4, #programsource*4, nil)
		local data = ffi.cast('int*', db:getdata())
		for i, token in ipairs(programsource) do
			data[i-1] = token
		end
		index.program = program:translate(db, true)
		-- local db = rsutil.loadptype(dbptype, 'program.bin')
		-- index.program = program:load(db, false)
	end

	index.shape = shapegroup:create()
	index.cshape = clearshape:create(true, false, false)
	index.cshape:setcolor(ffi.new('float[4]', 0, 0, 0, 1))
	index.pshape = primitiveshape:create(3, 1, 0, 0)
	index.vbuf = staticvertexbuffer:create(vdecl)
	index.vbuf:assign(vdata)
	index.pshape:setvertexbuffer(index.vbuf)
	index.pshape:setprogram(index.program)
	index.cshapeentry = index.shape:insert(index.cshape)
	index.pshapeentry = index.shape:insert(index.pshape)
	window:setshape(index.shape)

	while not index.program:isready() do
	end
	log('pb:isvalid', index.program:isvalid())
	local logdb = index.program:getlog()
	local logstr
	if logdb then
		logstr = ffi.string(logdb:getdata(), logdb:getlength())
	end
	log('pb:getlog', logstr)
	if index.program:isvalid() then
		local bindb = index.program:getbinary()
		if bindb then
			log('binary available')
			rsutil.saveptype(dbptype, bindb, 'program.bin')
		end
	end
	index.program:clearcache()
end
collectgarbage()

local function updatepos(event)
	if event.a == 1 then
		local x = event.b * (2 / info.width) - 1
		local y = event.c * (-2 / info.height) + 1
		index.pshape:setmatrix(ffi.new('float[16]',
			1, 0, 0, 0,
			0, 1, 0, 0,
			0, 0, 1, 0,
			x, y, 0, 1))
		local rx = x / 2 + 0.5
		local ry = y / 2 + 0.5
		index.cshape:setcolor(ffi.new('float[4]',
			math.pow(rx, 1/2.2), math.pow(ry, 1/2.2), 0, 1))
	end
end

input.register(input.events.pointdown, updatepos)
input.register(input.events.pointmove, updatepos)

input.maincycle()
