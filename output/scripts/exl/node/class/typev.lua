local modname = ...
local etypedef = package.relrequire(modname, 2, 'expr.typedef')
local eclassdef = etypedef:module(modname)
local common
local classinfo
local fulltype

function eclassdef:init(pr)
	etypedef.init(self, pr)
	self.classname = pr.classname
	self.parent = pr.parent
end

function eclassdef:build(pc)
	if self.parent then
		self.parent:build(pc)
	end
	self.classinfo = classinfo:create{
		context = pc,
		classname = self.classname,
		-- parent = self.parent,
	}
	self.fulltype = fulltype:create(self.classinfo:getdefti(), false, true)
	self.typeinfo = self.classinfo:getinstanceti()
end

function eclassdef:rcompile(stream)
	if not self.retname then
		self.retname = stream:genname()
		stream:writetoken(
			'v_class', self.retname, 0, self.classinfo:getclassname())
	end
	return self.retname
end

common = package.relrequire(modname, 3, 'common')
classinfo = package.relrequire(modname, 2, 'class')
fulltype = package.relrequire(modname, 3, 'fulltype')
