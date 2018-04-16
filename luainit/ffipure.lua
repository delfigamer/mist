local modname = 'base.ffipure'
local object = require('base.object')
local ffipure = object:module(modname)
local ffi = require('ffi')

ffipure.typedef = ''

function ffipure:derive(...)
	local child = object.derive(self, ...)
	child.metatype = nil
	child.new = ffipure.new
	return child
end

local function ffipure_new_next(self, ...)
	return self.metatype(...)
end

function ffipure:new(...)
	self:buildmetatype()
	return self.metatype(...)
end

function ffipure:buildmetatype()
	self.new = ffipure_new_next
	local suc, ret = pcall(ffi.metatype, self.typedef, self.instmeta)
	if suc then
		self.metatype = ret
	else
		ret.message = string.format('cannot create ctype for %s:\n%s',
			self._NAME, ret.message)
		error(ret)
	end
end

function ffipure.instmeta:__gc()
	return self:release()
end
