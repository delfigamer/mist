local modname = ...
local constent = require('exl.entity.const')
local exlcontext = require('exl.context')
local namespace = require('exl.namespace')
local valuebase = require('exl.value.base')
local unitt = require('exl.type.unit')
local unitent = require('exl.entity.unit')
local stdlibv = package.modtable(modname, valuebase:create{})

stdlibv.isconst = true

stdlibv.namespace = namespace:create()
stdlibv.context = exlcontext:create{
	environment = stdlibv.namespace}
stdlibv.type = unitt:create{
	context = stdlibv.context,
	unit = stdlibv}
stdlibv.entity = unitent:create{
	context = stdlibv.context,
	name = 'stdlib',
	value = stdlibv}

function stdlibv:defstring(lp)
	return 'unit stdlib'
end

for i, name in ipairs{
	'int'} do
	constent:create{
		context = stdlibv.context,
		name = name,
		value = require('exl.stdlib.' .. name)}
end
