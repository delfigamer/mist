local modname = ...
local ffipure = require('base.ffipure')
local triangle = ffipure:module(modname)
local ffi = require('ffi')
local vector = require(modname, 1, 'vector')

ffi.cdef[[
typedef struct {
	vector_t a;
	vector_t b;
	vector_t c;
} triangle_t;
]]

triangle.typedef = 'triangle_t'

function triangle:create(...)
	local inst = self:new(...)
	inst:init()
	return inst
end

function triangle:init()
end

triangle:buildmetatype()
