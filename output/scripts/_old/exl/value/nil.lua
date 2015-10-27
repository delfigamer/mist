local modname = ...
local base = require('exl.type.base')
local nilv = base:module(modname)

function nilv:init(it)
	base.init(self, it)
	self.type = self
end

function nilv:defstring(lp)
	return 'nil'
end

function nilv:compile_luai(lp)
	return 'nil'
end
