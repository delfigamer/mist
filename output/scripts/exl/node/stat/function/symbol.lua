local modname = ...
local symbase = require(modname, 4, 'symbol.base')
local symfunctor = symbase:module(modname)
local efunctor
local common
local fulltype
local functorti

function symfunctor:init(it)
	symbase.init(self, it)
	local ti = functorti:create{
		name = it.name,
	}
	self.fulltype = fulltype:create(ti, false, true)
	self.constvalue = efunctor:create{
		fulltype = self.fulltype,
	}
end

function symfunctor:rcompile(stream)
	return 0
end

function symfunctor:defstring(lp)
	if self.fulltype then
		return string.format('function %s',
			self.fulltype:defstring(lp))
	else
		return string.format('function <error>')
	end
end

efunctor = require(modname, 1, 'value')
common = require(modname, 4, 'common')
fulltype = require(modname, 4, 'fulltype')
functorti = require(modname, 1, 'ti')
