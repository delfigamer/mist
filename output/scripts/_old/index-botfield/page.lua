local modname = ...
local object = require('base.object')
local page = object:module(modname)
local botfield = require('index-botfield.botfield')
local clearshape = require('host.clearshape')
local ffi = require('ffi')
local fileio = require('host.fileio')
local info = require('host.info')
local input = require('system.input')
local iostream = require('host.iostream')
-- local mainconf = require('main')
local pixelprogram = require('host.pixelprogram')
local pngreadtask = require('host.pngreadtask')
local resourceset = require('rs.resourceset')
local shapegroup = require('host.shapegroup')
local spritefield = require('host.spritefield')
local window = require('host.window')

ffi.cdef[[
	typedef struct{ uint8_t r,g,b,a; } rgba8;
]]

local function obtainfieldmatrix()
	local io = fileio:create(_PATH .. 'resources/botfield/field.png', 'rb')
	local stream = iostream:create(io, 0, 'read')
	local task = pngreadtask:create(stream)
	task:flush()
	local width, height, si = task:getwidth(), task:getheight(), task:getdata()
	local bmdata = ffi.cast('rgba8*', si:getdata())
	local fdata = ffi.new('uint8_t[?]', width * height)
	for i = 0, width*height-1 do
		fdata[i] = bmdata[i].r
	end
	return width, height, fdata
end

function page:init()
	self.shape = shapegroup:create()
	self.clearshape = clearshape:create()
	self.clearshape:setcolor(0, 0, 0, 1)
	self.shape:insert(self.clearshape, -10)
	self.textures = {
		field = resourceset:loadresource('botfield.field', 'Texture'):gettexture(),
		bot = resourceset:loadresource('botfield.bot', 'Texture'):gettexture(),
	}

	local fmwidth, fmheight, fmdata = obtainfieldmatrix()
	self.field = botfield.field:create(fmwidth, fmheight, fmdata, io.open(_PATH .. 'scripts/index-botfield/script.lua'):read('*a'))
	self.field:setbotpos(130, 514, 0)
	self.field:setenginepos(7, 13)
	self.field:setsensorpos(22, 6, 22, -6)

	self.pixelprogram = pixelprogram:create()
	self.pixelprogram:setstagecolor(0, pixelprogram.Op_SelectArg1, pixelprogram.Arg_Texture, pixelprogram.Arg_Current)
	self.pixelprogram:setstagealpha(0, pixelprogram.Op_SelectArg1, pixelprogram.Arg_Texture, pixelprogram.Arg_Current)

	self.fieldsf = spritefield:create()
	self.shape:insert(self.fieldsf, 0)
	self.fieldsf:setpixelprogram(self.pixelprogram)
	self.fieldsf:settexture(0, self.textures.field)
	self.fieldsprite = self.fieldsf:newsprite(0)
	self.fieldsprite:settexregion1(0, 0, 1, 1)
	self.fieldsprite:setextent(fmwidth/2, 0, 0, fmheight/2)
	self.fieldsprite:setposition(fmwidth/2, fmheight/2)

	self.botsf = spritefield:create()
	self.shape:insert(self.botsf, 10)
	self.botsf:setpixelprogram(self.pixelprogram)
	self.botsf:settexture(0, self.textures.bot)
	self.botsf:setblendmethod(spritefield.Method_AlphaBlend)
	self.botsprite = self.botsf:newsprite(0)
	self.botsprite:settexregion1(0, 0, 1, 1)

	self.viewtarget = ffi.new('float[2]', fmwidth/2, fmheight/2)
	self.viewscale = 2
	self:updateview()
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

function page:updatebotposition()
	local botpos = self.field:getbotpos()
	self.botsprite:setposition(botpos[0], botpos[1])
	self.botsprite:setextent(
		math.cos(botpos[2])*32, math.sin(botpos[2])*32,
		-math.sin(botpos[2])*32, math.cos(botpos[2])*32)
end

function page:updateview()
	local matrix = ffi.new('float[16]',
		self.viewscale/info.width, 0, 0, 0,
		0, self.viewscale/info.height, 0, 0,
		0, 0, 1, 0,
		(-self.viewtarget[0]*self.viewscale-info.texelsoffset*2)/info.width, (-self.viewtarget[1]*self.viewscale-info.texeltoffset*2)/info.height, 0, 1)
	-- print(matrix[0], matrix[1], matrix[2], matrix[3])
	-- print(matrix[4], matrix[5], matrix[6], matrix[7])
	-- print(matrix[8], matrix[9], matrix[10], matrix[11])
	-- print(matrix[12], matrix[13], matrix[14], matrix[15])
	self.fieldsf:setmatrix(matrix)
	self.botsf:setmatrix(matrix)
	self:updatebotposition()
end

function page:onkeydown(message, key)
	if key == input.k.down then
		self.viewscale = self.viewscale / 2
		self:updateview()
	elseif key == input.k.up then
		self.viewscale = self.viewscale * 2
		self:updateview()
	end
end

function page:onpointdown(message, point, x, y)
	if point == 1 then
		self.viewtargetorigin = self.viewtarget
		self.point1origin = ffi.new('int[2]', x, y)
	end
end

function page:onpointmove(message, point, x, y)
	if point == 1 then
		local dx, dy = x - self.point1origin[0], y - self.point1origin[1]
		self.viewtarget = ffi.new('float[2]', self.viewtargetorigin[0] - dx*2/self.viewscale, self.viewtargetorigin[1] + dy*2/self.viewscale)
		self:updateview()
	end
end

function page:ontick(message, dt)
	self.field:tick(dt)
	self:updatebotposition()
end
