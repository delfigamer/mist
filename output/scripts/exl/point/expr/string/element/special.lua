local modname = ...
module(modname, package.seeall)

chartable = {
	n = '\n',
	r = '\r',
	t = '\t',
	['\''] = '\'',
	['"'] = '"',
	['\\'] = '\\',
	['\n'] = '',
}

function createpoint(self, pr, context)
	return chartable[pr._value]
end
