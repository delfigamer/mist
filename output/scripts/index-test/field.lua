local modname = ...
local object = require('base.object')
local field = object:module(modname)
local blendmode = require('host.enum.blendmode')
local ffi = require('ffi')
local flare = require('index-test.flare')
local info = require('host.info')
local mistsh = require('mistsh')
local set = require('base.set')
local shader = require('host.shader')
local spritefield = require('graphics.spritefield')
local util = require('rs.util')

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
		return float4(tv.aaa, 1) * color
	]]
	local tres = mistsh.translate(source)
	local sh = shader:create()
	sh:setshadersources(1, tres.vertex, tres.fragment, tres.texnames)
	return sh
end

function field:init()
	self.flares = set:create()
	self.spritefield = spritefield:create()
	self.time = 0
	local tex =
		util.loadasset(require('rs.asset.texture'), 'flare')
		:gettexresource()
	local shader = getshader()
	shader:settexture(0, tex)
	local shape = self:getshaperesource()
	shape:setshader(shader)
	shape:setblendmethod(blendmode.AddSaturated)
	shape:setmatrix(ffi.new('float[16]',
		2/info.width, 0, 0, 0,
		0, 2/info.height, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1))
end

function field:getshaperesource()
	return self.spritefield:getmsresource()
end

function field:createflare(...)
	local fl = flare:create(self, ...)
	self.flares:insert(fl)
	return fl
end

function field:removeflare(fl)
	self.flares:remove(fl)
end

function field:tick(dt)
	self.time = self.time + dt
	if self.time > 0.01 then
		self:update()
		self.time = self.time - 0.01
		if self.time > 0.01 then
			self.time = 0
		end
	end
end

function field:update()
	for i, fl in self.flares:pairs() do
		fl:update()
	end
	self.spritefield:update()
end

function field:gettarget()
	return self.target_x, self.target_y
end

function field:settarget(x, y)
	self.target_x = x
	self.target_y = y
end
