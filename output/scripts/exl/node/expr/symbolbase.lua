local modname = ...
local ebase = require('exl.node.expr.base')
local esymbolbase = ebase:module(modname)
local common

function esymbolbase:init(pr)
	ebase.init(self, pr)
end

function esymbolbase:build(pc)
	if self.target then
		self.fulltype = self.target:getfulltype()
		self.constvalue = self.target:getconstvalue()
	end
end

function esymbolbase:lcompile(stream, source)
	if self.target then
		self.target:lcompile(stream, source)
	end
end

function esymbolbase:rcompile(stream)
	if self.constvalue and self.constvalue.bsimplevalue then
		return self.constvalue:rcompile(stream)
	elseif self.target then
		return self.target:rcompile(stream)
	end
end

common = require('exl.common')
