local modname = ...
local eliteral = package.relrequire(modname, 1, 'literal')
local enil = eliteral:module(modname, {
	serial = 'v',
	fullname = 'nil',
})

function enil:init(pr)
	eliteral.init(self, pr)
end

function enil:rcompile(stream)
	return 0
end

function enil:defstring(lp)
	return 'nil'
end
