local modname = ...
local valuebase = require('exl.value.base')
local reference = valuebase:module(modname)

function reference:init(it)
	valuebase.init(self, it)
end

function reference:settarget(target)
	self.target = target
	self.type = target.type
	self.isconst = target.isconst
end

function reference:getconstvalue()
	return self.target.value and
		self.target.value:getconstvalue()
end

function reference:defstring(lp)
	return '<reference value>'
end
