local modname = ...
local etypedef = package.relrequire(modname, 2, 'typedef')
local efunctiontypev = etypedef:module(modname)
local common
local context
local functionti

function efunctiontypev:init(pr)
	etypedef.init(self, pr)
	self.arglist = pr.arglist
	self.rettype = pr.rettype
end

function efunctiontypev:dobuild(pc)
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

common = package.relrequire(modname, 4, 'common')
context = package.relrequire(modname, 4, 'context')
functionti = package.relrequire(modname, 1, 'ti')
