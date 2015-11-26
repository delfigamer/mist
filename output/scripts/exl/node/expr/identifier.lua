local modname = ...
local ebase = require('exl.node.expr.base')
local eidentifier = ebase:module(modname)
local common

function eidentifier:init(pr)
	ebase.init(self, pr)
	self.targetname = pr.targetname
end

function eidentifier:build(pc)
	self.target = pc:getsymbol(self.targetname)
	if not self.target then
		pc.env:log(
			'error',
			string.format('unknown identifier: %s', self.targetname),
			self.spos, self.epos)
		return
	end
	self.constvalue = self.target:getconstvalue()
end

function eidentifier:getfulltype()
	if self.target then
		return self.target:getfulltype()
	else
		return ebase.getfulltype(self)
	end
end

function eidentifier:getconstvalue()
	return self.constvalue
end

function eidentifier:lcompile(stream, source)
	if self.target then
		self.target:lcompile(stream, source)
	end
end

function eidentifier:rcompile(stream)
	if self.constvalue then
		return self.constvalue:rcompile(stream)
	elseif self.target then
		return self.target:rcompile(stream)
	end
end

function eidentifier:defstring(lp)
	return self.targetname
end

common = require('exl.common')
