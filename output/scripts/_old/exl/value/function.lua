local modname = ...
local valuebase = require('exl.value.base')
local exlcontext
local namespace
local point
local functionv = valuebase:module(modname)

functionv.isconst = true

function functionv:init(it)
	valuebase.init(self, it)
	self.namespace = it.namespace
	self.owncontext = it.owncontext
	self.rettype = it.rettype:constvalue()
	self.args = it.args
	self.body = it.body
end

function functionv:defstring(lp)
	local arglines = {}
	for i, arg in ipairs(self.args) do
		arglines[i] = arg:defstring(lp)
	end
	return 'function '..self.rettype:defstring(lp)..
		'('..table.concat(arglines, ', ')..')'..
		self.body:defstring(lp..self.tabchar)..'\
'..lp..'end'
end

exlcontext = require('exl.context')
namespace = require('exl.namespace')
--nilv = require('exl.value.nil')
point = require('exl.point.point')
