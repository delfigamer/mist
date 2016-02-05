local modname = ...
local object = require('base.object')
local ffipure = object:module(modname)
local ffi = require('ffi')

ffipure.fields = ''

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
	if not rawget(self, 'metatype') then
		self.new = ffipure_new_next
		self:buildmetatype()
	end
	return self.metatype(...)
end

function ffipure:buildmetatype()
	if self.cname then
		ffi.cdef(string.format('typedef struct{%s} %s', self.fields, self.cname))
		self.metatype = ffi.metatype(self.cname, self.instmeta)
	else
		local cdef = string.format('struct{%s}', self.fields)
		self.metatype = ffi.metatype(cdef, self.instmeta)
	end
end

function ffipure.instmeta:__gc()
	self:release()
end
