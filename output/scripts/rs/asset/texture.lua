local modname = ...
local asset = require('rs.asset')
local texture = asset:module(modname)
local arrayptype = require('rs.arrayptype')
local persistent = require('rs.persistent')
local pngptype = require('rs.pngptype')
local scalars = require('rs.scalars')
local texturedata = persistent:module(
	modname .. ':data',
	{
		{'minfilter', scalars.int, default = true},
		{'magfilter', scalars.int, default = true},
		{'wrapmodex', scalars.int, default = true},
		{'wrapmodey', scalars.int, default = true},
		{'levels', arrayptype(pngptype)},
	}
)
local databuffer = require('host.databuffer')
local ffi = require('ffi')
local statictexture
if require('host.methodlist').graphics ~= nil then
	statictexture = require('host.statictexture')
end

texturedata.minfilter = 5 -- LinearMipLinear
texturedata.magfilter = 1 -- Linear
texturedata.wrapmodex = 0 -- Repeat
texturedata.wrapmodey = 0 -- Repeat

local function box(a, b, c, d, aa, ba, ca, da)
	local weight = aa+ba+ca+da
	if weight > 0 then
		return (a*aa + b*ba + c*ca + d*da)/weight
	else
		return 0
	end
end

local function box_fmap(fmap, x, y, ch)
	return box(
		fmap[2*x][2*y][ch],
		fmap[2*x+1][2*y][ch],
		fmap[2*x][2*y+1][ch],
		fmap[2*x+1][2*y+1][ch],
		fmap[2*x][2*y][3],
		fmap[2*x+1][2*y][3],
		fmap[2*x][2*y+1][3],
		fmap[2*x+1][2*y+1][3]
	)
end

local function generatemiplevels(bm)
	local levels = {}
	levels[1] = bm
	local width = bm.width
	local height = bm.height
	local fmap = ffi.typeof('float[$][$][4]', width, height)()
	local ptr = ffi.cast('uint8_t const*', bm.db:getdata())
	for y = 0, height-1 do
		local row = ptr + y * width * 4
		for x = 0, width-1 do
			local pixel = row + x * 4
			fmap[x][y][0] = math.pow(pixel[0]/255, 2.2)
			fmap[x][y][1] = math.pow(pixel[1]/255, 2.2)
			fmap[x][y][2] = math.pow(pixel[2]/255, 2.2)
			fmap[x][y][3] = pixel[3]/255
		end
	end
	local levelindex = 1
	while width > 1 and height > 1 do
		levelindex = levelindex + 1
		width = bit.rshift(width + 1, 1)
		height = bit.rshift(height + 1, 1)
		local db = databuffer:create(width*height*4, width*height*4)
		local ptr = ffi.cast('uint8_t*', db:getdata())
		for y = 0, height-1 do
			local row = ptr + y * width * 4
			for x = 0, width-1 do
				fmap[x][y][0] = box_fmap(fmap, x, y, 0)
				fmap[x][y][1] = box_fmap(fmap, x, y, 1)
				fmap[x][y][2] = box_fmap(fmap, x, y, 2)
				fmap[x][y][3] = (
					fmap[2*x][2*y][3] + fmap[2*x+1][2*y][3] +
					fmap[2*x][2*y+1][3] + fmap[2*x+1][2*y+1][3]) / 4
				local pixel = row + x * 4
				pixel[0] = math.pow(fmap[x][y][0], 1/2.2)*255
				pixel[1] = math.pow(fmap[x][y][1], 1/2.2)*255
				pixel[2] = math.pow(fmap[x][y][2], 1/2.2)*255
				pixel[3] = fmap[x][y][3]*255
			end
		end
		levels[levelindex] = {
			width = width,
			height = height,
			db = db,
		}
	end
	return levels
end

function texturedata:init(bm)
	if
		bit.band(bm.width, bm.width-1) ~= 0
		or bit.band(bm.height, bm.height-1) ~= 0
	then
		error('only power-of-two textures are supported')
	end
	self.levels = generatemiplevels(bm)
end

function texturedata:initload()
end

function texture:init(bm)
	self.data = texturedata:create(bm)
end

function texture:loaddata(stream)
	return texturedata.ptype:read(stream)
end

function texture:initload(data)
	self.data = data
	if self.texresource then
		self.texresource:assign(
			0, data.levels[1].db, data.levels[1].width, data.levels[1].height)
		self.texresource:setminfilter(data.minfilter)
		self.texresource:setmagfilter(data.magfilter)
		self.texresource:setwrapmode(data.wrapmodex, data.wrapmodey)
	end
end

function texture:loadfailed()
	self.data = false
end

function texture:release()
	if self.texresource then
		self.texresource:release()
	end
end

function texture:isready()
	return self.data ~= nil
end

function texture:savedata(stream)
	if self.data then
		texturedata.ptype:write(self.data, stream)
	else
		error('invalid texture object')
	end
end

function texture:gettexresource()
	if not statictexture then
		error('graphics are not supported')
	end
	if not self.texresource then
		self.texresource = statictexture:create()
		if self.data then
			self.texresource:assign(
				0,
				self.data.levels[1].db,
				self.data.levels[1].width,
				self.data.levels[1].height)
			self.texresource:setminfilter(self.data.minfilter)
			self.texresource:setmagfilter(self.data.magfilter)
			self.texresource:setwrapmode(self.data.wrapmodex, self.data.wrapmodey)
		end
	end
	return self.texresource
end

function texture:getminfilter()
	if self.data then
		return self.data.minfilter
	end
end

function texture:setminfilter(value)
	if self.data then
		self.data.minfilter = value
	end
	if self.texresource then
		self.texresource:setminfilter(value)
	end
end

function texture:getmagfilter()
	if self.data then
		return self.data.magfilter
	end
end

function texture:setmagfilter(value)
	if self.data then
		self.data.magfilter = value
	end
	if self.texresource then
		self.texresource:setmagfilter(value)
	end
end

function texture:getwrapmode()
	if self.data then
		return self.data.wrapmodex, self.data.wrapmodey
	end
end

function texture:setwrapmode(wx, wy)
	if self.data then
		self.data.wrapmodex = wx
		self.data.wrapmodey = wy
	end
	if self.texresource then
		self.texresource:setwrapmode(wx, wy)
	end
end
