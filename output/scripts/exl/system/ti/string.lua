local modname = ...
local typeinfo = require('exl.typeinfo')
local stringti = package.modtable(modname, typeinfo:create())

function stringti:defstring(lp)
	return 'string'
end
