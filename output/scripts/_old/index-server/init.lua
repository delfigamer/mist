local modname = ...
module(modname, package.seeall)
local page = require('index-server.page')

function register()
	currentpage = page:create()
	currentpage:register()
end

function unregister()
	currentpage:unregister()
	currentpage = nil
end
