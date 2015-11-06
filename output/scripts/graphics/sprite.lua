local modname = ...
local object = require('base.object')
local sprite = object:module(modname)
local databuffer = require('host.databuffer')
local ffi = require('ffi')
local ffipure = require('base.ffipure')
local meshdata = require('host.meshdata')
local set = require('base.set')
local types = require('host.types')

local spritedata_t = ffi.typeof([[
struct {
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
}]])

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
--    Quad ABCD is texture-mapped to rectangle, corresponding to ranges
-- [tex_x1; tex_x2) and [tex_y1; tex2).
--    There are two separate texture mappings, governed by tex1_* and tex2_*
-- sets of fields, which are later passed as "texcoord1" and "texcoord2"
-- to shaders.
--    Colors are passed directly to shaders, so their exact meaning depends on
-- how a particular shader uses them.

function sprite:init(field)
	self.field = field
	self.data = spritedata_t()
end

function sprite:release()
	self.field:removesprite(self)
end

function sprite:getcenter()
	return
		self.data.center_x,
		self.data.center_y
end

function sprite:setcenter(x, y)
	self.data.center_x = x
	self.data.center_y = y
end

function sprite:getextent_x()
	return
		self.data.extent_xx,
		self.data.extent_xy
end

function sprite:setextent_x(x, y)
	self.data.extent_xx = x
	self.data.extent_xy = y
end

function sprite:getextent_y()
	return
		self.data.extent_yx,
		self.data.extent_yy
end

function sprite:setextent_y(x, y)
	self.data.extent_yx = x
	self.data.extent_yy = y
end

function sprite:gettexregion1()
	return
		self.data.tex1_x1,
		self.data.tex1_y1,
		self.data.tex1_x2,
		self.data.tex1_y2
end

function sprite:settexregion1(x1, y1, x2, y2)
	self.data.tex1_x1 = x1
	self.data.tex1_y1 = y1
	self.data.tex1_x2 = x2
	self.data.tex1_y2 = y2
end

function sprite:gettexregion2()
	return
		self.data.tex2_x1,
		self.data.tex2_y1,
		self.data.tex2_x2,
		self.data.tex2_y2
end

function sprite:settexregion2(x1, y1, x2, y2)
	self.data.tex2_x1 = x1
	self.data.tex2_y1 = y1
	self.data.tex2_x2 = x2
	self.data.tex2_y2 = y2
end

function sprite:getcolor()
	return
		self.data.color_r,
		self.data.color_g,
		self.data.color_b,
		self.data.color_a
end

function sprite:setcolor(r, g, b, a)
	self.data.color_r = r
	self.data.color_g = g
	self.data.color_b = b
	self.data.color_a = a
end
