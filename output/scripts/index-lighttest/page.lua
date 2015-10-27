local modname = ...
local object = require('base.object')
local page = object:module(modname)
-- local conf = require('config.index-lighttest')
local ffi = require('ffi')
local info = require('host.info')
local input = require('system.input')
local inputhandler = require('index-lighttest.inputhandler')
local clearshape = require('host.clearshape')
local pixelprogram = require('host.pixelprogram')
local rendertarget = require('host.rendertarget')
local resourceset = require('rs.resourceset')
local shapegroup = require('host.shapegroup')
local spritefield = require('host.spritefield')
local texture = require('host.texture')
local types = require('host.types')
local window = require('host.window')

local floormatrix = ffi.new('float[16]',
	info.width/512, 0, 0, 0,
	0, info.height/512, 0, 0,
	0, 0, 1, 0,
	0.5 + info.texelsoffset/256, 0.5 + info.texeltoffset/256, 0, 1)

function page:init()
	self.inputhandler = inputhandler:create(self)
	self.shape = shapegroup:create()
	self.clearshape = clearshape:create()
	self.clearshape:setcolor(0, 0, 0, 1)
-- 	self.shape:insert(self.clearshape, -10)
	self.width = info.width
	self.height = info.height
	self.textures = {
		floor_normal = resourceset:loadresource('lighttest.floor_normal', 'Texture'):gettexture(),
		light_normal = resourceset:loadresource('common.spot_normal_60', 'Texture'):gettexture(),
		floor_diffuse = resourceset:loadresource('lighttest.floor_diffuse', 'Texture'):gettexture(),
		floor_emissive = resourceset:loadresource('lighttest.floor_emissive', 'Texture'):gettexture(),
	}
-- 	self.lightrt = rendertarget:create(1024, 1024)
-- 	self.lightrt:setminfilter(texture.MinFilter_Nearest)
-- 	self.lightrt:setmagfilter(texture.MagFilter_Nearest)
-- 	self.lightrt:setclearcolor(true, types.colortype(0, 0, 0, 1))
	self.lightpp = pixelprogram:create()
	self.lightpp:setmatrix(pixelprogram.Matrix_Mat1, floormatrix)
	
	self.lightpp:setstagetexture(0, self.textures.floor_normal)
	self.lightpp:setstagetexcoord(0, pixelprogram.TexCoord_Screen)
	self.lightpp:setstagecolor(0, pixelprogram.Op_SelectArg1, pixelprogram.Arg_Texture, pixelprogram.Arg_Current)
	self.lightpp:setstagematrix(0, pixelprogram.Matrix_Mat1)
	
	self.lightpp:setstagetexture(1, self.textures.light_normal)
	self.lightpp:setstagetexcoord(1, pixelprogram.TexCoord_Tex1)
	self.lightpp:setstagecolor(1, pixelprogram.Op_DotProduct, pixelprogram.Arg_Texture, pixelprogram.Arg_Current)
	
	self.lightpp:setstagecolor(2, pixelprogram.Op_Modulate, pixelprogram.Arg_Diffuse, pixelprogram.Arg_Current)
	self.lightpp:setstagematrix(2, pixelprogram.Matrix_Mat1)
	
	self.lightpp:setstagetexture(3, self.textures.floor_diffuse)
	self.lightpp:setstagecolor(3, pixelprogram.Op_Modulate, pixelprogram.Arg_Texture, pixelprogram.Arg_Current)
	self.lightpp:setstagetexcoord(3, pixelprogram.TexCoord_Screen)
	self.lightpp:setstagematrix(3, pixelprogram.Matrix_Mat1)
	
	self.lightsf = spritefield:create()
	self.lightsf:setpixelprogram(self.lightpp)
	self.lightsf:setblendmethod(spritefield.Method_AddSaturated)
-- 	self.lightrt:setshape(self.lightsf)
	self.light_msprite = self.lightsf:newsprite(0)
	self.light_msprite:settexregion1(0, 0, 1, 1)
	self.light_msprite:setextent(64/self.width, 0, 0, 64/self.height)
	self.time = 0
	self.lights = {}
	self.floorpp = pixelprogram:create()
	self.floorpp:setmatrix(pixelprogram.Matrix_Mat1, floormatrix)
	
-- 	self.floorpp:setstagetexture(0, self.lightrt)
-- 	self.floorpp:setstagecolor(0, pixelprogram.Op_SelectArg1, pixelprogram.Arg_Texture, pixelprogram.Arg_Current)
-- 	self.floorpp:setstagetexcoord(0, pixelprogram.TexCoord_Tex1)
	
	self.floorpp:setstagetexture(0, self.textures.floor_emissive)
	self.floorpp:setstagecolor(0, pixelprogram.Op_SelectArg1, pixelprogram.Arg_Texture, pixelprogram.Arg_Current)
	self.floorpp:setstagetexcoord(0, pixelprogram.TexCoord_Screen)
	self.floorpp:setstagematrix(0, pixelprogram.Matrix_Mat1)
	
	self.floorsf = spritefield:create()
	self.floorsf:setpixelprogram(self.floorpp)
	self.floorsf:setblendmethod(spritefield.Method_Replace)
	self.shape:insert(self.floorsf, 0)
	self.floorsprite = self.floorsf:newsprite(0)
	self.floorsprite:settexregion1(0, 0, 1, -1)
	self.floorsprite:setextent(1, 0, 0, 1)
	self.shape:insert(self.lightsf, 10)
end

function page:register()
	self.inputhandler:register()
	window:setshape(self.shape)
end

function page:unregister()
	window:setshape()
	self.inputhandler:unregister()
end

local period
do
	local pamp = 0.05
	local poffset = 0.95
	local pfreq = 2*math.pi*16
	function period(arg)
		arg = arg*pfreq
		local tr = math.sin(arg) * pamp + poffset
		local tg = math.sin(arg + math.pi*2/3) * pamp + poffset
		local tb = math.sin(arg + math.pi*4/3) * pamp + poffset
		return math.pow(tr, 2.2), math.pow(tg, 2.2), math.pow(tb, 2.2), 1
	end
end

function page:onpointmove(msg, point, x, y)
	local sx, sy = 2*x/self.width - 1, 1 - 2*y/self.height
	self.light_msprite:setposition(sx, sy)
end

function page:onpointdown(msg, point, x, y)
	local sx, sy = 2*x/self.width - 1, 1 - 2*y/self.height
	if point == 1 then
		local light = self.lightsf:newsprite(10)
		light:settexregion1(0, 0, 1, 1)
		light:setextent(64/self.width, 0, 0, 64/self.height)
		light:setcolor(types.colortype(0.7, 0.7, 0.7, 1))
		light:setposition(sx, sy)
		self.lights[#self.lights + 1] = light
		print('lights:', #self.lights)
	end
end

function page:onpointup(msg, point)
	if point == 1 then
	end
end

function page:ontick(msg, dt)
	self.time = self.time + dt
	self.light_msprite:setcolor(types.colortype(period(self.time)))
end
