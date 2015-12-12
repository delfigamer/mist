local modname = ...
local typeinfo = require('exl.typeinfo')
local stringti = package.modtable(modname, typeinfo:create())

stringti.serial = 's'

function stringti:defstring(lp)
	return 'string'
end
