local modname = ...
module(modname, package.seeall)
local input = require('system.input')
local page = require('index-editor.page')

function register()
	currentpage = page:create()
	currentpage:register()
end

function unregister()
	currentpage:unregister()
	currentpage = nil
end
