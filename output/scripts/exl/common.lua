local modname = ...
module(modname, package.seeall)
local default

function createpoint(pr, ...)
	local success, result = pcall(require, 'exl.point.' .. pr.name)
	if success then
		return result:createpoint(pr, ...)
	else
		print(result)
		return default:createpoint(pr, ...)
	end
end

default = require('exl.defaultpoint')
