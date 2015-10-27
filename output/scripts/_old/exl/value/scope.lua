local modname = ...
local scopev = package.modtable(modname)

function scopev:create(it)
	local base = it.base
	local basetype = assert(base.type,
		'scoped value must have a type')
	return basetype:scope(it)
end
