local modname = ...
module(modname, package.seeall)

function createpoint(self, pr, context)
	return string.char(tonumber(pr._value, 16))
end
