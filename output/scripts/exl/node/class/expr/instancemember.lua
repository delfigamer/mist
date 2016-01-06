local modname = ...
local ereference = package.relrequire(modname, 3, 'expr.reference')
local einstancemember = ereference:module(modname)
local common

function einstancemember:init(pr)
	ereference.init(self, pr)
	self.base = pr.base
	self.index = pr.index
end

function einstancemember:lcompile(stream, source)
	local basename = self.base:rcompile(stream)
	self.target:lcompile(stream, source, basename)
end

function einstancemember:rcompile(stream)
	if self.constvalue and self.constvalue.bsimplevalue then
		return self.constvalue:rcompile(stream)
	else
		if not self.retname then
			local basename = self.base:rcompile(stream)
			self.retname = self.target:rcompile(stream, basename)
		end
		return self.retname
	end
end

function einstancemember:defstring(lp)
	return string.format('%s.%s',
		self.base:defstring(lp),
		common.identstring(self.index))
end

common = package.relrequire(modname, 4, 'common')
