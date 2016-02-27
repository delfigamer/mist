local modname = ...
local ref = require('host.ref')
local texture = ref:module(modname, 'texture', 'Texture')
local ffi = require('ffi')
local host = require('host')
local ml = require('host.methodlist').graphics

if ml == nil then
	error('graphics are not supported')
end

function texture:setminfilter(value)
	if not ml.texture_setminfilter(self.ptr, value) then
		host.checkerror()
	end
end

function texture:setmagfilter(value)
	if not ml.texture_setmagfilter(self.ptr, value) then
		host.checkerror()
	end
end

function texture:setwrapmode(ws, wt)
	if not ml.texture_setwrapmode(self.ptr, ws, wt) then
		host.checkerror()
	end
end
