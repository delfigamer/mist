local modname = ...
local base = require('exl.entity.base')
local errors
local const = base:module(modname)

const.isconst = true

function const:init(it)
	base.init(self, it)
	self.value = it.value:constvalue()
	assert(self.value.isconst,
		errors.const_expected{
			value = self.value})
	self.type = self.value.type
end

function const:constvalue()
	return self.value:constvalue()
end

function const:defstring(lp)
	return 'const '..self.value:defstring(lp)
end

errors = require('exl.errors')
