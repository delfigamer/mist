local modname = ...
local ebase = require('exl.node.expr.base')
local esubexpression = ebase:module(modname)
local common

function esubexpression:init(pr)
	ebase.init(self, pr)
	self.value = pr.value
end

function esubexpression:build(pc)
	if self.value then
		self.value:build(pc)
	end
end

function esubexpression:getfulltype()
	if self.value then
		return self.value:getfulltype()
	else
		return ebase.getfulltype(self)
	end
end

function esubexpression:getconstvalue()
	if self.value then
		return self.value:getconstvalue()
	end
end

function esubexpression:lcompile(stream, source)
	if self.value then
		return self.value:lcompile(stream, source)
	end
end

function esubexpression:rcompile(stream)
	if self.value then
		return self.value:rcompile(stream)
	end
end

function esubexpression:defstring(lp)
	return string.format('--[[%s]] (%s)',
		common.defstring(self:getfulltype(), lp .. self.lpindent),
		common.defstring(self.value, lp .. self.lpindent))
end

common = require('exl.common')
