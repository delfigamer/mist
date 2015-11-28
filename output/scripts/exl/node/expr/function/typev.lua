local modname = ...
local etypebase = require('exl.node.expr.typebase')
local efunctiontypev = etypebase:module(modname)
local common
local context
local functionti

function efunctiontypev:init(pr)
	etypebase.init(self, pr)
	self.arglist = pr.arglist
	self.rettype = pr.rettype
end

function efunctiontypev:build(pc)
	self.context = context:create(pc)
	self.arglist:build(self.context)
	if self.rettype then
		self.rettype:build(self.context)
	end
	self.typeinfo = functionti:create{
		arglist = self.arglist,
		rettype = self.rettype,
	}
end

function efunctiontypev:defstring(lp)
	return functionti.defstring(self, lp)
end

common = require('exl.common')
context = require('exl.context')
functionti = require('exl.node.expr.function.ti')
