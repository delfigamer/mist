local modname = ...
local ebase = require(modname, 1, 'base')
local esubexpression = ebase:module(modname)
local common

function esubexpression:init(pr)
	ebase.init(self, pr)
	self.value = pr.value
end

function esubexpression:dobuild(pc)
	self.value:build(pc)
	self.fulltype = self.value:getfulltype()
	self.constvalue = self.value:getconstvalue()
end

function esubexpression:lcompile(stream, source)
	return self.value:lcompile(stream, source)
end

function esubexpression:rcompile(stream)
	return self.value:rcompile(stream)
end

function esubexpression:defstring(lp)
	return string.format('(%s)',
		self.value:defstring(lp .. self.lpindent))
end

common = require(modname, 3, 'common')
