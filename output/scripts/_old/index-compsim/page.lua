local modname = ...
local object = require('base.object')
local page = object:module(modname)
local blendmode = require('host.enum.blendmode')
local clearshape = require('host.clearshape')
local complex = require('complex')
local databuffer = require('host.databuffer')
local ffi = require('ffi')
local info = require('host.info')
local input = require('system.input')
local mainconf = require('main')
local mistsh = require('mistsh')
local shader = require('host.shader')
local shapegroup = require('host.shapegroup')
local statictexture = require('host.statictexture')
local spritefield = require('graphics.spritefield')
local utility = require('base.utility')
local window = require('host.window')

local matsize = mainconf.matsize or 32
local imgscale = mainconf.imgscale or 8

local function clampf(c)
	return (math.abs(c) - math.abs(c-1) + 1) * 0.5
end

local function writestate(matrix, t)
	local db = databuffer:create(matsize*matsize*4, matsize*matsize*4)
	local ptr = ffi.cast('uint8_t*', db:getdata())
	for x = 0, matsize-1 do
		for y = 0, matsize-1 do
			local v = matrix[x][y]
			local h = v:arg() * (3 / math.pi)
			local r = clampf(2 - math.abs(h))
			local g = clampf(2 - math.abs(h-2)) + clampf(2 - math.abs(h+4))
			local b = clampf(2 - math.abs(h+2)) + clampf(2 - math.abs(h-4))
			local s = clampf(v:abs())
			r, g, b = r*s, g*s, b*s
			ptr[y*(matsize*4)+x*4+0] = r*255
			ptr[y*(matsize*4)+x*4+1] = g*255
			ptr[y*(matsize*4)+x*4+2] = b*255
			ptr[y*(matsize*4)+x*4+3] = 255
		end
	end
	return 0, db, matsize, matsize
end

local res_factor = complex:createpolar(0.9, math.pi/200)
local adv_factor = complex:createpolar(0.1/4, math.pi/200)

local function advancematrix(source, target)
	target[0][0] =
		source[0][0] * res_factor +
		source[matsize-1][0] * adv_factor +
		source[1][0] * adv_factor +
		source[0][matsize-1] * adv_factor +
		source[0][1] * adv_factor
	for y = 1, matsize-2 do
		target[0][y] =
			source[0][y] * res_factor +
			source[matsize-1][y] * adv_factor +
			source[1][y] * adv_factor +
			source[0][y-1] * adv_factor +
			source[0][y+1] * adv_factor
	end
	target[0][matsize-1] =
		source[0][matsize-1] * res_factor +
		source[matsize-1][matsize-1] * adv_factor +
		source[1][matsize-1] * adv_factor +
		source[0][matsize-2] * adv_factor +
		source[0][0] * adv_factor
	for x = 1, matsize-2 do
		target[x][0] =
			source[x][0] * res_factor +
			source[x-1][0] * adv_factor +
			source[x+1][0] * adv_factor +
			source[x][matsize-1] * adv_factor +
			source[x][1] * adv_factor
		for y = 1, matsize-2 do
			target[x][y] =
				source[x][y] * res_factor +
				source[x-1][y] * adv_factor +
				source[x+1][y] * adv_factor +
				source[x][y-1] * adv_factor +
				source[x][y+1] * adv_factor
		end
		target[x][matsize-1] =
			source[x][matsize-1] * res_factor +
			source[x-1][matsize-1] * adv_factor +
			source[x+1][matsize-1] * adv_factor +
			source[x][matsize-2] * adv_factor +
			source[x][0] * adv_factor
	end
	target[matsize-1][0] =
		source[matsize-1][0] * res_factor +
		source[matsize-2][0] * adv_factor +
		source[0][0] * adv_factor +
		source[matsize-1][matsize-1] * adv_factor +
		source[matsize-1][1] * adv_factor
	for y = 1, matsize-2 do
		target[matsize-1][y] =
			source[matsize-1][y] * res_factor +
			source[matsize-2][y] * adv_factor +
			source[0][y] * adv_factor +
			source[matsize-1][y-1] * adv_factor +
			source[matsize-1][y+1] * adv_factor
	end
	target[matsize-1][matsize-1] =
		source[matsize-1][matsize-1] * res_factor +
		source[matsize-2][matsize-1] * adv_factor +
		source[0][matsize-1] * adv_factor +
		source[matsize-1][matsize-2] * adv_factor +
		source[matsize-1][0] * adv_factor
end

local function getshader()
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
		float4 tv = tex2D(tex, texcoord1)
		return tv
	]]
	local tres = mistsh.translate(source)
	local sh = shader:create()
	sh:setshadersources(1, tres.vertex, tres.fragment, tres.texnames)
	return sh
end


function page:init()
	self.shapegroup = shapegroup:create()
	self.clearshape = clearshape:create()
	self.clearshape:setcolor(0, 0, 0, 1)
	self.cliterator = self.shapegroup:insert(self.clearshape, -10)
	self.mat1 = ffi.new(string.format('complex_t[%i][%i]', matsize, matsize))
	self.mat2 = ffi.new(string.format('complex_t[%i][%i]', matsize, matsize))
	self.mtime = 0
	self.spritefield = spritefield:create()
	self.time = 0
	self.texture = statictexture:create()
	self.texture:setmagfilter(0)
	local shader = getshader()
	shader:settexture(0, self.texture)
	local shape = self.spritefield:getmsresource()
	shape:setshader(shader)
	shape:setblendmethod(blendmode.Replace)
	shape:setmatrix(ffi.new('float[16]',
		2/info.width, 0, 0, 0,
		0, 2/info.height, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1))
	self.sfiterator = self.shapegroup:insert(shape, 0)
	self.sprite = self.spritefield:createsprite()
	self.sprite:setcenter(0, 0)
	self.sprite:setextent_x(matsize*imgscale, 0)
	self.sprite:setextent_y(0, matsize*imgscale)
	self.sprite:settexregion1(0, 0, 1, 1)
	self.spritefield:update()
end

function page:register()
	input.handlers[2] = self
	window:setshape(self.shapegroup)
end

function page:unregister()
	input.handlers[2] = nil
end

function page:handle(message, ...)
	local messagehandler = self['handle_' .. message]
	if messagehandler then
		messagehandler(self, message, ...)
	end
end

function page:handle_tick(message, dt)
	self.time = self.time + dt
	if self.time > 0.1 then
		self:update()
		self.time = self.time - 0.1
		if self.time > 0.1 then
			self.time = 0
		end
	end
end

function page:update()
	self.mtime = self.mtime + 1
	advancematrix(self.mat1, self.mat2)
	self.mat1, self.mat2 = self.mat2, self.mat1
	self.mat1[0][0] = self.mat1[0][0] + complex:createpolar(4, 0)
	self.texture:assign(writestate(self.mat1))
	print(self.mtime, string.format('%.2f', self.mat1[0][0]:abs()))
end
