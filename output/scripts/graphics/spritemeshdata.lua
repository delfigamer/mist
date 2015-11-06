local modname = ...
local object = require('base.object')
local spritemeshdata = object:module(modname)
local databuffer = require('host.databuffer')
local ffi = require('ffi')
local ffipure = require('base.ffipure')
local meshdata = require('host.meshdata')
local set = require('base.set')
local types = require('host.types')

local function clampf(c)
	return (math.abs(c) - math.abs(c-1) + 1) * 0.5
end

local function argb8(r, g, b, a)
	local rb = math.floor(clampf(r)*255)
	local gb = math.floor(clampf(g)*255)
	local bb = math.floor(clampf(b)*255)
	local ab = math.floor(clampf(a)*255)
	return bit.bor(
		bb,
		bit.lshift(gb, 8),
		bit.lshift(rb, 16),
		bit.lshift(ab, 24))
end

local function fillvertices(sp, pvertex)
	local color = argb8(sp:getcolor())
	local center_x, center_y = sp:getcenter()
	local extent_xx, extent_xy = sp:getextent_x()
	local extent_yx, extent_yy = sp:getextent_y()
	local tex1_x1, tex1_y1, tex1_x2, tex1_y2 = sp:gettexregion1()
	local tex2_x1, tex2_y1, tex2_x2, tex2_y2 = sp:gettexregion2()
	pvertex[0].pos_x = center_x - extent_xx - extent_yx
	pvertex[0].pos_y = center_y - extent_xy - extent_yy
	pvertex[0].pos_z = 0
	pvertex[0].tex1_x = tex1_x1
	pvertex[0].tex1_y = tex1_y1
	pvertex[0].tex2_x = tex2_x1
	pvertex[0].tex2_y = tex2_y1
	pvertex[0].color = color
	pvertex[1].pos_x = center_x - extent_xx + extent_yx
	pvertex[1].pos_y = center_y - extent_xy + extent_yy
	pvertex[1].pos_z = 0
	pvertex[1].tex1_x = tex1_x1
	pvertex[1].tex1_y = tex1_y2
	pvertex[1].tex2_x = tex2_x1
	pvertex[1].tex2_y = tex2_y2
	pvertex[1].color = color
	pvertex[2].pos_x = center_x + extent_xx + extent_yx
	pvertex[2].pos_y = center_y + extent_xy + extent_yy
	pvertex[2].pos_z = 0
	pvertex[2].tex1_x = tex1_x2
	pvertex[2].tex1_y = tex1_y2
	pvertex[2].tex2_x = tex2_x2
	pvertex[2].tex2_y = tex2_y2
	pvertex[2].color = color
	pvertex[3].pos_x = center_x + extent_xx - extent_yx
	pvertex[3].pos_y = center_y + extent_xy - extent_yy
	pvertex[3].pos_z = 0
	pvertex[3].tex1_x = tex1_x2
	pvertex[3].tex1_y = tex1_y1
	pvertex[3].tex2_x = tex2_x2
	pvertex[3].tex2_y = tex2_y1
	pvertex[3].color = color
end

function spritemeshdata:init()
	self.mdresource = meshdata:create()
	self.sprites = set:create()
end

function spritemeshdata:release()
	self.mdresource:release()
end

function spritemeshdata:insertsprite(sp)
	self.sprites:insert(sp)
end

function spritemeshdata:removesprite(sp)
	self.sprites:remove(sp)
end

local function printmbcontents(mb)
	local vdb = mb:getvertexdata()
	if vdb then
		print(string.format('vertex data, size: %#x', vdb:getlength()))
		local vptr = ffi.cast(types.pvertex, vdb:getdata())
		for i = 0, math.floor(vdb:getlength()/ffi.sizeof(types.vertex)) - 1 do
			print(string.format([[
vertex %i:
	pos_x = %.4f
	pos_y = %.4f
	pos_z = %.4f
	tex1_x = %.4f
	tex1_y = %.4f
	tex2_x = %.4f
	tex2_y = %.4f
	color = 0x%.8x]],
				i,
				vptr[i].pos_x,
				vptr[i].pos_y,
				vptr[i].pos_z,
				vptr[i].tex1_x,
				vptr[i].tex1_y,
				vptr[i].tex2_x,
				vptr[i].tex2_y,
				vptr[i].color))
		end
	else
		print('no vertices')
	end
	local idb = mb:getindexdata()
	if idb then
		print(string.format('index data, size: %#x', idb:getlength()))
		local iptr = ffi.cast('uint16_t*', idb:getdata())
		for i = 0, math.floor(idb:getlength()/2) - 1 do
			print(string.format('\tindex %i: %i', i, iptr[i]))
		end
	else
		print('no indices')
	end
end

local function update_main(self, mb)
	local vdb = mb:getvertexdata()
	local vdsize = 4 * ffi.sizeof(types.vertex) * self.sprites.count
	if not vdb or vdb:getcapacity() < vdsize then
		vdb = databuffer:create(vdsize, bit.band(vdsize + 255, 0xffffff00))
		mb:setvertexdata(vdb)
	else
		vdb:setlength(vdsize)
	end
	local vptr = ffi.cast(types.pvertex, vdb:getdata())
	for i, sprite in self.sprites:pairs() do
		fillvertices(sprite, vptr + 4 * (i - 1))
	end

	local idb = mb:getindexdata()
	local idsize = 2 * 6 * self.sprites.count
	if not idb or idb:getcapacity() < idsize then
		idb = databuffer:create(idsize, bit.band(idsize + 255, 0xffffff00))
		mb:setindexdata(idb)
	else
		idb:setlength(idsize)
	end
	local iptr = ffi.cast('uint16_t*', idb:getdata())
	for i = 0, self.sprites.count-1 do
		iptr[6*i+0] = 4*i+0
		iptr[6*i+1] = 4*i+3
		iptr[6*i+2] = 4*i+2
		iptr[6*i+3] = 4*i+0
		iptr[6*i+4] = 4*i+2
		iptr[6*i+5] = 4*i+1
	end
	mb:setindexdata(idb)
end

function spritemeshdata:update()
	local mb = self.mdresource:lock()
	local suc, ret = pcall(update_main, self, mb)
	mb:release()
	if not suc then
		error(ret)
	end
end

function spritemeshdata:getmdresource()
	return self.mdresource
end
