local modname = ...
module(modname, package.seeall)
local types = require('host.types')

function uuidtoname(uuid)
	return string.format(':%.8x%.8x', uuid.hi, uuid.lo)
end

function nametouuid(name)
	name = name:match('^:([0-9a-f]*)$')
	if not name or #name ~= 16 then
		return nil
	end
	return types.itemuuid(tonumber(name:sub(1,8), 16), tonumber(name:sub(9,16), 16))
end
