local modname = ...
local etypedef = require(modname, 2, 'expr.typedef')
local eclassdef = etypedef:module(modname)
local common
local classinfo
local fulltype

function eclassdef:init(pr)
	etypedef.init(self, pr)
	self.classname = pr.classname
	self.parent = pr.parent
end

function eclassdef:dobuild(pc)
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
		stream:append{
			'newclass',
			{'ssa', 0}, -- parent
			{'string', self.classinfo:getclassname()}, -- classname
			{'ssa', self.retname}, -- target
		}
	end
	return self.retname
end

common = require(modname, 3, 'common')
classinfo = require(modname, 2, 'class')
fulltype = require(modname, 3, 'fulltype')
