local modname = ...
module(modname, package.seeall)

function createpoint(self, pr, context)
	return pr._char
end
