local modname = ...
local shape = require('host.shape')
local shapegroup = shape:module(modname, 'shapegroup', 'ShapeGroup')
local ffi = require('ffi')
local ffipure = require('base.ffipure')
local host = require('host')
local ml = require('host.methodlist').graphics

if ml == nil then
	error('graphics are not supported')
end

local iterator = ffipure:derive(nil, modname .. ':iterator')

iterator.fields = 'ShapeGroup* shapegroup; ShapeGroup_iterator iterator;'

function iterator:release()
	if self.shapegroup ~= nil then
		ml.shapegroup_remove(self.shapegroup, self.iterator)
		self.shapegroup = nil
	end
end

function shapegroup:create()
	local ptr = ml.shapegroup_new()
	if ptr ~= nil then
		return self:new(ptr)
	else
		host.checkerror()
		return nil
	end
end

function shapegroup:insert(item, order)
	local it = iterator:new()
	if not ml.shapegroup_insert(self.ptr, item and item.ptr, order or 0, it.iterator) then
		host.checkerror()
		return nil
	end
	it.shapegroup = self.ptr
	return it
end

function shapegroup:setactive(active)
	if not ml.shapegroup_setactive(self.ptr, active) then
		host.checkerror()
	end
end
