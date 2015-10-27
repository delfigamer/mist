local modname = ...
local object = require('base.object')
local spritefield = object:module(modname)
local databuffer = require('host.databuffer')
local meshdata = require('host.meshdata')
local meshshape = require('host.meshshape')

local function argb8(r, g, b, a)
	local rb = r > 1 and 1 or r < 0 and 0 or math.floor(r*255)
	local gb = g > 1 and 1 or g < 0 and 0 or math.floor(g*255)
	local bb = b > 1 and 1 or b < 0 and 0 or math.floor(b*255)
	local ab = a > 1 and 1 or a < 0 and 0 or math.floor(a*255)
	return bit.bor(
		bb,
		bit.lshift(gb, 8),
		bit.lshift(rb, 16),
		bit.lshift(ab, 24))
end

local vertices = {}
for x = 0, 100 do
	for y = 0, 100 do
		vertices[x*101+y] = types.vertex(
			x/50-1, y/50-1, 0,
			x/100 + info.texelsoffset/512, y/100 + info.texeltoffset/512,
			0, 0,
			argb8(x/100, y/100, 0, 1))
	end
end

local indices = {}
do
	local index = 0
	for x = 0, 99 do
		for y = 0, 99 do
			indices[index] = x*101+y
			indices[index+1] = (x+1)*101+y
			indices[index+2] = (x+1)*101+(y+1)
			indices[index+3] = x*101+y
			indices[index+4] = (x+1)*101+(y+1)
			indices[index+5] = x*101+(y+1)
			index = index + 6
		end
	end
end

local function initmeshdata(meshdata)
	local mb = meshdata:lock()
	local vdb = databuffer:create(ffi.sizeof(types.vertex) * (#vertices+1), ffi.sizeof(types.vertex) * (#vertices+1), nil)
	local vptr = ffi.cast(types.pvertex, vdb:getdata())
	for i = 0, #vertices do
		vptr[i] = vertices[i]
	end
	mb:setvertexdata(vdb)
	local idb = databuffer:create(2 * (#indices+1), 2 * (#indices+1), nil)
	local iptr = ffi.cast('uint16_t*', idb:getdata())
	for i = 0, #indices do
		iptr[i] = indices[i]
	end
	mb:setindexdata(idb)
	mb:release()
end

function spritefield:init()
	self.shape = meshshape:create()
	self.meshdata = meshdata:create()
	initmeshdata(self.meshdata)
	self.meshshape:setmeshdata(self.meshdata)
	self.meshshape:setshader(self.shader)
	self.meshshape:setblendmethod(meshshape.Method_AlphaBlend)
	self.meshshape:setmatrix(ffi.new('float[16]',
		512/info.width, 0, 0, (512 + 64)/info.width - 1,
		0, 512/info.height, 0, 1 - (512 + 64)/info.height,
		0, 0, 1, 0,
		0, 0, 0, 1))
	self.shader:settexture(0, self.texture)
	self.meshshapeiterator = self.shape:insert(self.meshshape)
end

function page:register()
	input.handlers[2] = self
	window:setshape(self.shape)
end

function page:unregister()
	window:setshape()
	input.handlers[2] = nil
end

function page:handle(message, ...)
	local hf = self['on'..message]
	if hf then
		hf(self, message, ...)
	end
end

function page:onpointmove(msg, point, x, y)
	if point == 1 then
		local xf = x/info.width * 2 - 1
		local yf = 1 - y/info.height * 2
		local matrix = ffi.new('float[16]',
			xf, 0, 0, 0,
			0, yf, 0, 0,
			0, 0, 1, 0,
			0, 0, 0, 1)
		self.meshshape:setmatrix(matrix)
	end
end

function page:onpointdown(msg, point, x, y)
end

function page:onpointup(msg, point)
end

function page:ontick(msg, dt)
end
