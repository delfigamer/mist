local modname = ...
local ffipure = require('base.ffipure')
local location = ffipure:module(modname)
local serialize = require('base.serialize')

location.fields = [[
	float x;
	float y;
	float yaw;]]

function location:create(...)
	return self:new(...)
end

function location.instmeta:__tostring()
	return string.format('location(%f, %f, %f)', self.x, self.y, self.yaw)
end

function location.instmeta:__serialize()
	return tostring(self)
end

function serialize.senv.location(x, y, yaw)
	return location:create(x, y, yaw)
end
