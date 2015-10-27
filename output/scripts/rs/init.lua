local modname = ...
local rs = package.modtable(modname)
local util = require('rs.util')

rs.loaded = {}

function rs.load(typename, name)
	local classname = 'rs.asset.' .. typename
	local loaded = rs.loaded[name]
	if loaded then
		if loaded['#' .. classname] then
			return loaded
		end
	else
		local class = require(classname)
		local asset = util.loadasset(class, name)
		rs.loaded[name] = asset
		return asset
	end
end
