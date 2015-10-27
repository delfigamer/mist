local modname = ...
local valuebase = require('exl.value.base')
local exlcontext
local namespace
local functionv = valuebase:module(modname)
local farglist = require('exl.value.function.farglist')

functionv.isconst = true

function functionv:init(it)
	valuebase.init(self, it)
	self.namespace = it.namespace
	self.innercontext = it.innercontext
	self.rettype = it.rettype:constvalue()
	self.args = farglist:create{
		args = it.args}
	self.body = it.body
end

function functionv:defstring(lp)
	return 'function '..self.rettype:defstring(lp)..
		self.args:defstring(lp)..
		self.body:defstring(lp..self.tabchar)..'\
'..lp..'end'
end

function functionv:compile_luai(lp)
	return 'function'..self.args:compile_luai(lp)..
		self.body:compile_luai(lp..self.tabchar)..'\
'..lp..'end'
end

exlcontext = require('exl.context')
namespace = require('exl.namespace')
