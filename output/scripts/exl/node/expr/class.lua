local modname = ...
local etypedef = package.relrequire(modname, 1, 'typedef')
local eclassdef = etypedef:module(modname)
local common
local classcontext
local classti

function eclassdef:init(pr)
	etypedef.init(self, pr)
	self.parent = pr.parent
	self.body = pr.body
end

function eclassdef:build(pc)
	if self.parent then
		self.parent:build(pc)
	end
-- inheritance is not implemented, so the parent value is ignored
	self.context = classcontext:create(nil, pc)
	self.typeinfo = classti:create{
		parent = self.parent,
		body = self.body,
	}
	self.body:build(pc)
end

function eclassdef:defstring(lp)
	return classti.defstring(self, lp)
end

common = package.relrequire(modname, 3, 'common')
classcontext = package.relrequire(modname, 2, 'class.context')
classti = package.relrequire(modname, 2, 'class.ti')
