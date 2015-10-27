local modname = ...
local defbase = require('exl.point.def.base')
local vare
local point
local var = defbase:module(modname, 'def.var')

function var:init(pr, context)
	defbase.init(self, pr, context)
	self.type = point:create(pr._type, context)
	self.name = pr._name
	self.entity = vare:create{
		context = context,
		type = self.type:getvalue(),
		name = self.name}
	if pr._value then
		self.value = point:create(pr._value, context)
		self.cvalue = self.entity.type:acast(
			self.value:getvalue())
	end
end

function var:defstring(lp)
	local vstr
	if self.value then
		vstr = ' = '..self.value:defstring(lp)
	else
		vstr = ''
	end
	return 'var '..self.type:defstring(lp)..' '..self.name..
		vstr
end

function var:compile_luai(lp)
	local edef
	if self.visibility == 'public' then
		edef = self.entity.innername
	else
		edef = 'local '..self.entity.innername
	end
	local vstr
	if self.value then
		vstr = ' = '..self.value:compile_luai(lp)
	else
		vstr = ''
	end
	return edef..vstr
end

vare = require('exl.entity.var')
point = require('exl.point.base')
