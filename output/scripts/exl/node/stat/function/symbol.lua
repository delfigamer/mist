local modname = ...
local symbase = package.relrequire(modname, 4, 'symbol.base')
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

efunctor = package.relrequire(modname, 1, 'value')
common = package.relrequire(modname, 4, 'common')
fulltype = package.relrequire(modname, 4, 'fulltype')
functorti = package.relrequire(modname, 1, 'ti')
