local modname = ...
local vare = require('exl.entity.var')
local farge = vare:module(modname)

function farge:init(it)
	local vit = {
		context = it.context,
		type = it.type}
	if it.name then
		vit.name = it.name
	else
		vit.innername = 'arg_'..tostring(it.index)
	end
	vare.init(self, vit)
end

function farge:defstring(lp)
	local nstr
	if self.name then
		nstr = ' '..self.name
	else
		nstr = ''
	end
	return self.type:defstring(lp)..nstr
end
