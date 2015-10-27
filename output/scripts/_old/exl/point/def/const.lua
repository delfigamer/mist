local modname = ...
local constbase = require('exl.point.def.base')
local conste
local point
local const = constbase:module(modname, 'def.const')

function const:init(pr, context)
	constbase.init(self, pr, context)
	self.name = pr._name
	self.value = point:create(pr._value, context)
	self.entity = conste:create{
		context = context,
		visibility = self.visibility,
		name = self.name,
		value = self.value:getvalue()}
end

function const:defstring(lp)
	return 'const '..self.name..' = '..self.value:defstring(lp)
end

function const:compile_luai(lp)
	local edef
	if self.visibility == 'public' then
		edef = self.entity.innername
	else
		edef = 'local '..self.entity.innername
	end
	local value = self.entity.value
	return edef..' = '..value:compile_luai(lp)
end

conste = require('exl.entity.const')
point = require('exl.point.base')
