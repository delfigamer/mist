local modname = ...
local valuebase = require('exl.value.base')
local stdlib_intt
local int = valuebase:module(modname)

int.isconst = true
int.format = '%d'

function int:init(it)
	valuebase.init(self, it)
	self.type = stdlib_intt
	self.intvalue = it.intvalue
	self.format = it.format
end

function int:defstring(lp)
	return string.format(self.format, self.intvalue)
end

function int:compile_luai(lp)
	return string.format(self.format, self.intvalue)
end

stdlib_intt = require('exl.stdlib.int')
