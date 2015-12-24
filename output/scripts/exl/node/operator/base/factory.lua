local modname = ...
local object = package.relrequire(modname, 4, 'object')
local baseof = object:module(modname)

function baseof:init(pr)
end

function baseof:createinstance(it)
end
