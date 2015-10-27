local modname = ...
local reference = require('exl.value.reference')
local identref = reference:module(modname)

function identref:init(it)
	reference.init(self, it)
	self.name = it.name
	self.entity = self.context.environment:geta(self.name)
	self:settarget(self.entity)
end

function identref:defstring(lp)
	return self.name
end

function identref:compile_luai(lp)
	return self.entity.innername
end
