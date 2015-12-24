local modname = ...
local typeinfo = package.relrequire(modname, 2, 'base.ti')
local typedefti = package.modtable(modname, typeinfo:create())

typedefti.serial = 't'

function typedefti:defstring(lp)
	return 'typedef'
end
