local modname = ...
local object = require('base.object')
local spritemeshdata = object:module(modname)
local databuffer = require('host.databuffer')
local ffi = require('ffi')
local ffipure = require('base.ffipure')
local meshdata = require('host.meshdata')
local set = require('base.set')
local quality = require('graphics.quality')
local types = require('host.types')

local sprite = ffipure:derive(nil, modname .. ':sprite')

sprite.fields = [[
	float center_x;
	float center_y;
	float extent_xx;
	float extent_xy;
	float extent_yx;
	float extent_yy;
	float tex1_x1;
	float tex1_y1;
	float tex1_x2;
	float tex1_y2;
	float tex2_x1;
	float tex2_y1;
	float tex2_x2;
	float tex2_y2;
	float color_r;
	float color_g;
	float color_b;
	float color_a;
]]

-- B ( 1 )   F         C ( 2 )
-- ..........^..........
--  .         \         .
--   .         \         .
--    .         \         .
--     .         \         .
--      .         o---------> E
--       .        O          .
--        .                   .
--         .                   .
--          .                   .
--           .....................
--           A ( 0 )             D ( 3 )
--    O - (center_x; center_y)
--    OE - {extent_xx; extent_xy}
--    OF - {extent_yx; extent_yy}
--    Quad ABCD is texture-mapped to rectangle, which corresponds to ranges
-- [tex_x1; tex_x2) and [tex_y1; tex2).
--    There are two separate texture mappings, governed by tex1_* and tex2_*
-- sets of fields, which are later passed as "texcoord1" and "texcoord2"
-- to shaders.
--    Colors are passed directly to shaders, so their exact meaning depends on
-- how a particular shader uses them.

function sprite:init()
end

function sprite:release()
end

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

-- vertex order:
-- 1 2
-- 0 3
function sprite:fillvertices(pvertex)
	local color = argb8(self.color_r, self.color_g, self.color_b, self.color_a)
	pvertex[0].pos_x = self.center_x - self.extent_xx - self.extent_yx
	pvertex[0].pos_y = self.center_y - self.extent_xy - self.extent_yy
	pvertex[0].pos_z = 0
	pvertex[0].tex1_x = self.tex1_x1
	pvertex[0].tex1_y = self.tex1_y1
	pvertex[0].tex2_x = self.tex2_x1
	pvertex[0].tex2_y = self.tex2_y1
	pvertex[0].color = color
	pvertex[1].pos_x = self.center_x - self.extent_xx + self.extent_yx
	pvertex[1].pos_y = self.center_y - self.extent_xy + self.extent_yy
	pvertex[1].pos_z = 0
	pvertex[1].tex1_x = self.tex1_x1
	pvertex[1].tex1_y = self.tex1_y2
	pvertex[1].tex2_x = self.tex2_x1
	pvertex[1].tex2_y = self.tex2_y2
	pvertex[1].color = color
	pvertex[2].pos_x = self.center_x + self.extent_xx + self.extent_yx
	pvertex[2].pos_y = self.center_y + self.extent_xy + self.extent_yy
	pvertex[2].pos_z = 0
	pvertex[2].tex1_x = self.tex1_x2
	pvertex[2].tex1_y = self.tex1_y2
	pvertex[2].tex2_x = self.tex2_x2
	pvertex[2].tex2_y = self.tex2_y2
	pvertex[2].color = color
	pvertex[3].pos_x = self.center_x + self.extent_xx - self.extent_yx
	pvertex[3].pos_y = self.center_y + self.extent_xy - self.extent_yy
	pvertex[3].pos_z = 0
	pvertex[3].tex1_x = self.tex1_x2
	pvertex[3].tex1_y = self.tex1_y1
	pvertex[3].tex2_x = self.tex2_x2
	pvertex[3].tex2_y = self.tex2_y1
	pvertex[3].color = color
end

function spritemeshdata:init()
	self.mdresource = meshdata:create()
	self.sprites = set:create()
end

function spritemeshdata:release()
	self.mdresource:release()
end

function spritemeshdata:createsprite()
	local sp = sprite:create()
	self.sprites:insert(sp)
	return sp
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
		sprite:fillvertices(vptr + 4 * (i - 1))
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

	-- printmbcontents(mb)
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
