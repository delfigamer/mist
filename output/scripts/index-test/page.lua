local modname = ...
local object = require('base.object')
local page = object:module(modname)
local ffi = require('ffi')
local info = require('host.info')
local input = require('system.input')
local types = require('host.types')
local window = require('host.window')

local clearshape = require('host.clearshape')
local databuffer = require('host.databuffer')
local meshdata = require('host.meshdata')
local meshshape = require('host.meshshape')
local mistsh = require('mistsh')
local shader = require('host.shader')
local shapegroup = require('host.shapegroup')
local spritemeshdata = require('graphics.spritemeshdata')
local statictexture = require('host.statictexture')
local util = require('rs.util')

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

local function createtexture()
	local asset = util.loadasset(require('rs.asset.texture'), 'tex1')
	asset:blockuntilready()
	-- asset:setminfilter(0)
	-- asset:setmagfilter(0)
	asset:setwrapmode(1, 1)
	local tex = asset:gettexresource()
	return tex
end

local function initshader(sh)
	local source = [[
	varying float4 pos;
	varying float4 color;
	varying float2 texcoord1;

	vertex
		pos = a_position
		color = a_color
		texcoord1 = a_texcoord1.xy
		return mul(a_position, worldviewmatrix)

	fragment
		texture 0 tex
		return (tex2D(tex, texcoord1) * 0.7 + 0.3) * color
	]]
	-- print('________\n')
	-- print(source)
	local tres = mistsh.translate(source)
	-- print('________\n')
	-- print(tres.vertex)
	-- print('________\n')
	-- print(tres.fragment)
	-- print('________\n')
	-- print(string.format('%q', tres.texnames))
	-- print('________\n')
	sh:setshadersources(tres.vertex, tres.fragment, tres.texnames)
end

function page:init()
	self.shape = shapegroup:create()
	self.clearshape = clearshape:create()
	self.clearshape:setcolor(0, 0, 0, 1)
	self.clearshapeiterator = self.shape:insert(self.clearshape, -100)
	self.smd = spritemeshdata:create()
	self.sprite = self.smd:createsprite()
	self.sprite.center_x = 0
	self.sprite.center_y = 0
	self.sprite.extent_xx = 32
	self.sprite.extent_xy = 0
	self.sprite.extent_yx = 0
	self.sprite.extent_yy = 32
	self.sprite.tex1_x1 = 0
	self.sprite.tex1_x2 = 1
	self.sprite.tex1_y1 = 0
	self.sprite.tex1_y2 = 1
	-- self.sprite.tex2_x1 = 0.5
	-- self.sprite.tex2_x2 = 0.6
	-- self.sprite.tex2_y1 = 0.7
	-- self.sprite.tex2_y2 = 0.8
	-- self.sprite.color_r = x / info.width
	-- self.sprite.color_g = y / info.height
	-- self.sprite.color_b = 0
	-- self.sprite.color_a = 1
	self.smd:update()
	self.meshdata = self.smd:getmdresource()
	self.texture = createtexture()
	self.shader = shader:create()
	initshader(self.shader)
	self.meshshape = meshshape:create()
	self.meshshape:setmeshdata(self.meshdata)
	self.meshshape:setshader(self.shader)
	self.meshshape:setblendmethod(meshshape.Method_AlphaBlend)
	self.meshshape:setmatrix(ffi.new('float[16]',
		2/info.width, 0, 0, 2/info.width - 1,
		0, 2/info.height, 0, 1 - 2/info.height,
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
		self.sprite.center_x = x
		self.sprite.center_y = -y
		self.sprite.color_r = x / info.width
		self.sprite.color_g = y / info.height
		self.sprite.color_b = 1
		self.sprite.color_a = 1
		self.smd:update()
	end
end

function page:onpointdown(msg, point, x, y)
end

function page:onpointup(msg, point)
end

function page:ontick(msg, dt)
end
