local modname = ...
local object = package.relrequire(modname, 1, 'object')
local env = object:module(modname)

function env:init()
end
