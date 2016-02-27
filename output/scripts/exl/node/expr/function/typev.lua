local modname = ...
local etypedef = require(modname, 2, 'typedef')
local efunctiontypev = etypedef:module(modname)
local common
local context
local functionti

function efunctiontypev:init(pr)
	etypedef.init(self, pr)
	self.args = pr.args
	self.rettype = pr.rettype
end

function efunctiontypev:dobuild(pc)
	self.context = context:create(pc)
	for i, arg in ipairs(self.args) do
		arg:build(self.context)
	end
	if self.rettype then
		self.rettype:build(self.context)
	end
	self.typeinfo = functionti:create{
		args = self.args,
		rettype = self.rettype,
	}
end

function efunctiontypev:defstring(lp)
	return functionti.defstring(self, lp)
end

common = require(modname, 4, 'common')
context = require(modname, 4, 'context')
functionti = require(modname, 1, 'ti')
