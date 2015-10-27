local modname = ...
local defbase = require('exl.point.def.base')
local farge
local point
local farg = defbase:module(modname, 'function.farg')

function farg:init(pr, context, index)
	defbase.init(self, pr, context)
	self.type = point:create(pr._type, context)
	self.name = pr._name
	self.index = index
	self.entity = farge:create{
		context = context,
		type = self.type:getvalue(),
		name = self.name,
		index = self.index}
end

function farg:defstring(lp)
	local nstr
	if self.name then
		nstr = ' '..self.name
	else
		nstr = ''
	end
	return self.type:defstring(lp)..nstr
end

farge = require('exl.entity.farg')
point = require('exl.point.base')
