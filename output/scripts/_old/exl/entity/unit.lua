local modname = ...
local base = require('exl.entity.base')
local unite = base:module(modname)

unite.isconst = true

function unite:init(it)
	base.init(self, it)
	self.value = it.value
	self.type = self.value.type
	self.innername = self.value.innername
end

function unite:getconstvalue()
	return self.value
end

function unite:defstring(lp)
	return self.value:defstring(lp)
end
