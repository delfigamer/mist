local modname = ...
local ffipure = require('base.ffipure')
local vector = ffipure:module(modname)
local ffi = require('ffi')

ffi.cdef[[
typedef struct {
	float x;
	float y;
	float z;
} vector_t;
]]

vector.typedef = 'vector_t'

function vector:create(...)
	return self:new(...)
end

vector:buildmetatype()
