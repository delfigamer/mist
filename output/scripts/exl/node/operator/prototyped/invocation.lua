local modname = ...
local invbase = require(modname, 2, 'base.invocation')
local invprototyped = invbase:module(modname)

function invprototyped:init(it)
	invbase.init(self, it)
	self.args = it.args
	self.fulltype = it.fulltype
	self.maxrank = it.maxrank
end

function invprototyped:rcompile(stream)
	return 0
end

function invprototyped:prefcompare(other)
	if not other['#'..modname] then
		return 0
	end
	for i, acast in ipairs(self.args) do
		local apc = acast:prefcompare(other.args[i])
		if apc ~= 0 then
			return apc
		end
	end
	return 0
end
