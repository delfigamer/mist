local modname = ...
local ref = require('host.ref')
local shader = ref:module(modname, 'shader', 'Shader')
local ffi = require('ffi')
local host = require('host')
local ml = require('host.methodlist').graphics

if ml == nil then
	error('graphics are not supported')
end

function shader:create()
	local ptr = ml.shader_new()
	if ptr ~= nil then
		return self:new(ptr)
	else
		host.checkerror()
		return nil
	end
end

function shader:settexture(stage, texture)
	if not ml.shader_settexture(self.ptr, stage, texture and texture.ptr) then
		host.checkerror()
	end
end

function shader:setshadersources(format, vert, frag, texnames)
	if not ml.shader_setshadersources(self.ptr, format, vert, frag, texnames) then
		host.checkerror()
	end
end
