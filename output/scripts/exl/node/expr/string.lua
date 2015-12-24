local modname = ...
local eliteral = package.relrequire(modname, 1, 'literal')
local estring = eliteral:module(modname, {
	serial = 's',
	fullname = 'string',
})

function estring:init(pr)
	eliteral.init(self, pr)
	self.value = pr.value
end

function estring:rcompile(stream)
	local name = stream:genname()
	stream:writetoken('v_string', name, self.value)
	return name
end

function estring:defstring(lp)
	return string.format('%q', self.value)
end
