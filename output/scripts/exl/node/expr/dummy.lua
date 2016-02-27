local modname = ...
local ebase = require(modname, 1, 'base')
local edummy = ebase:module(modname)
local common

function edummy:init(pr)
	ebase.init(self, pr)
	self.fulltype = pr.fulltype
end

function edummy:lcompile(stream, source)
	self.retname = source
end

function edummy:rcompile(stream)
	return self.retname
end

function edummy:defstring(lp)
	return string.format('< dummy: %s >', self.fulltype:defstring(lp))
end

common = require(modname, 3, 'common')
