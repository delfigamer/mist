local modname = ...
local ref = require('host.ref')
local shape = ref:module(modname, 'shape', 'Shape')
local ml = require('host.methodlist').graphics

if ml == nil then
	error('graphics are not supported')
end
