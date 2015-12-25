local modname = ...
local exlerror = package.relrequire(modname, 1, 'exlerror')
local nodeerror = exlerror:module(modname)

function nodeerror:init(message, node)
	exlerror.init(message, node.spos, node.epos, node.filename)
end

