local modname = ...
local typeinfo = require('exl.typeinfo')
local numberti = package.modtable(modname, typeinfo:create())

numberti.serial = 'n'

function numberti:defstring(lp)
	return 'number'
end
