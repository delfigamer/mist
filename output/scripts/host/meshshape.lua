local modname = ...
local shape = require('host.shape')
local meshshape = shape:module(modname, 'meshshape', 'MeshShape')
local ffi = require('ffi')
local host = require('host')
local ml = require('host.methodlist').graphics
local types = require('host.types')

if ml == nil then
	error('graphics are not supported')
end

function meshshape:create()
	local ptr = ml.meshshape_new()
	if ptr ~= nil then
		return self:new(ptr)
	else
		host.checkerror()
		return nil
	end
end

function meshshape:setmeshdata(md)
	if not ml.meshshape_setmeshdata(self.ptr, md and md.ptr) then
		host.checkerror()
	end
end

function meshshape:setshader(sh)
	if not ml.meshshape_setshader(self.ptr, sh and sh.ptr) then
		host.checkerror()
	end
end

function meshshape:setblendmethod(method)
	if not ml.meshshape_setblendmethod(self.ptr, method) then
		host.checkerror()
	end
end

function meshshape:settexture(stage, texture)
	if not ml.meshshape_settexture(self.ptr, stage, texture and texture.ptr) then
		host.checkerror()
	end
end

function meshshape:setmatrix(matrix)
	if not ml.meshshape_setmatrix(self.ptr, matrix) then
		host.checkerror()
	end
end

meshshape.Method_Replace = 0
meshshape.Method_AlphaBlend = 1
meshshape.Method_Add = 2
meshshape.Method_AddSaturated = 3
meshshape.Method_Invalid = 4
