local modname = ...
module(modname, package.seeall)
local input = require('system.input')
local page = package.relrequire(modname, 0, 'page')

function register()
	currentpage = page:create()
	currentpage:register()
end

function unregister()
	currentpage:unregister()
	currentpage = nil
end
