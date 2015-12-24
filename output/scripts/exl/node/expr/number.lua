local modname = ...
local eliteral = package.relrequire(modname, 1, 'literal')
local enumber = eliteral:module(modname, {
	serial = 'n',
	fullname = 'number',
})

function enumber:init(pr)
	eliteral.init(self, pr)
	self.value = pr.value
end

function enumber:rcompile(stream)
	local name = stream:genname()
	stream:writetoken('v_number', name, self.value)
	return name
end

function enumber:defstring(lp)
	return tostring(self.value)
end
