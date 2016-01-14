local modname = ...
local object = package.relrequire(modname, 2, 'object')
local symbase = object:module(modname)
local fulltype

function symbase:init(it)
	self.context = it.context
	self.defpos = it.defpos
	self.deffile = it.deffile
	self.fulltype = it.fulltype
	self.constvalue = it.constvalue
end

function symbase:getfulltype()
	return self.fulltype
end

function symbase:getconstvalue()
	return self.constvalue
end

fulltype = package.relrequire(modname, 2, 'node.expr.base').fulltype
