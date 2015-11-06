local modname = ...
local object = require('base.object')
local flare = object:module(modname)

function flare:init(field, x, y)
	self.field = field
	self.sprite = self.field.spritefield:createsprite()
	self.sprite:setcenter(x, y)
	self.sprite:setextent_x(16, 0)
	self.sprite:setextent_y(0, 16)
	self.sprite:settexregion1(0, 0, 1, 1)
	local a = math.random()
	self.sprite:setcolor(a, a, a, a)
	self.target_x = x
	self.target_y = y
end

function flare:release()
	self.field:removeflare(self)
end

local function swarm(s, t, wt, wr)
	local r = math.random() - math.random()
	return s * (1 - wt) + t * wt + r * wr
end

function flare:update()
	local r, g, b, a = self.sprite:getcolor()
	a = swarm(a, 0, 0.05, 1)
	r = math.atan(a)/math.pi+0.5
	self.sprite:setcolor(r, r, r, a)
	local x, y = self.sprite:getcenter()
	local tx, ty = self.target_x, self.target_y
	local fx, fy = self.field:gettarget()
	tx = swarm(tx, fx, 0.005, 20)
	ty = swarm(ty, fy, 0.005, 20)
	x = swarm(x, tx, 0.15, 2)
	y = swarm(y, ty, 0.15, 2)
	self.sprite:setcenter(x, y)
	self.target_x = tx
	self.target_y = ty
end
