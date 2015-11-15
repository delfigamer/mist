local modname = ...
local symbase = require('exl.symbol.base')
local symconst = symbase:module(modname)
local common

function symconst:init(it)
	symbase.init(self, it)
end

function symconst:defstring(lp)
	if self.constvalue then
		return string.format('%s const %s',
			self.id, common.defstring(self.constvalue, lp))
	else
		return string.format('%s const forward %s',
			self.id, common.defstring(self.type, lp))
	end
end

function symconst.instmeta:__tostring()
	return self:defstring('')
end

common = require('exl.common')
