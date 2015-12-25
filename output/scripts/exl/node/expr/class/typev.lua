local modname = ...
local etypedef = package.relrequire(modname, 2, 'typedef')
local eclassdef = etypedef:module(modname)
local common
local classinfo
local fulltype

function eclassdef:init(pr)
	etypedef.init(self, pr)
	self.classname = pr.classname
	self.parent = pr.parent
	self.body = pr.body
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
	self.body:build(self.classinfo:getcontext())
	self.fulltype = fulltype:create(self.classinfo:getdefti(), false, true)
	self.typeinfo = self.classinfo:getinstanceti()
end

function eclassdef:defstring(lp)
	if self.parent then
		return string.format('type class %s: %s%s\n%send',
			common.identstring(self.classname),
			self.parent:defstring(lp .. self.lpindent),
			self.body:defstring(lp .. self.lpindent),
			lp)
	else
		return string.format('type class %s%s\n%send',
			common.identstring(self.classname),
			self.body:defstring(lp .. self.lpindent),
			lp)
	end
end

common = package.relrequire(modname, 4, 'common')
classinfo = package.relrequire(modname, 3, 'class')
fulltype = package.relrequire(modname, 4, 'fulltype')
