local modname = ...
local _G = _G
module(modname, package.seeall)

function printtext(s)
	_G.print(s)
end

function print(...)
	local args = {...}
	for i = 1, select('#', ...) do
		args[i] = tostring(args[i])
	end
	printtext(table.concat(args, '\t'))
end

function log(...)
	_G.log(...)
	print(...)
end
