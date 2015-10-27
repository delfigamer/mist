local modname = ...
local base = require('exl.type.base')
local functiontype = base:module(modname)
local farglist = require('exl.value.function.farglist')

function functiontype:init(it)
	base.init(self, it)
	self.rettype = it.rettype
	self.arglist = farglist:create{
		args = it.args}
end

function functiontype:defstring(lp)
	return 'type function '..self.rettype:defstring(lp)..
		self.arglist:defstring(lp)
end

function functiontype:compile_luai(lp)
	return 'nil --[['..self:defstring(lp)..']]'
end
