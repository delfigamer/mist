local modname = ...
local eliteral = require(modname, 1, 'literal')
local enumber = eliteral:module(modname, {
	serial = 'n',
	fullname = 'number',
})
local common

function enumber:init(pr)
	eliteral.init(self, pr)
	self.value = pr.value
end

function enumber:rcompile(stream)
	local name = stream:genname()
	stream:append{
		[0]='move',
		{[0]='number', self.value}, -- source
		{[0]='ssa', name}, -- target
	}
	return name
end

function enumber:defstring(lp)
	return common.dtos(self.value)
end

common = require(modname, 3, 'common')
