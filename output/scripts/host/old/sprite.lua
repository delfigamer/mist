local modname = ...
local hostobject = require('host.hostobject')
local sprite = hostobject:module(modname, 'sprite', 'Sprite')
local ffi = require('ffi')
local host = require('host')
local ml = require('host.methodlist').graphics

sprite.fields = sprite.fields .. 'SpriteField* sf;'

function sprite:setposition(x, y)
	if not ml.sprite_setposition(self.ptr, x, y) then
		host.checkerror()
	end
end

function sprite:setextent(x1, y1, x2, y2)
	if not ml.sprite_setextent(self.ptr, x1, y1, x2, y2) then
		host.checkerror()
	end
end

function sprite:settexregion1(x1, y1, x2, y2)
	if not ml.sprite_settexregion1(self.ptr, x1, y1, x2, y2) then
		host.checkerror()
	end
end

function sprite:settexregion2(x1, y1, x2, y2)
	if not ml.sprite_settexregion2(self.ptr, x1, y1, x2, y2) then
		host.checkerror()
	end
end

function sprite:setcolor(color)
	if not ml.sprite_setcolor(self.ptr, color) then
		host.checkerror()
	end
end

function sprite:release()
	if self.ptr ~= nil then
		ml.spritefield_removesprite(self.sf, self.ptr)
		self.ptr = nil
	end
end
