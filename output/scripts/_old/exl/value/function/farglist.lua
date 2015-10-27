local modname = ...
local object = require('base.object')
local farglist = object:module(modname)

function farglist:init(it)
	self.args = it.args
end

function farglist:defstring(lp)
	local arglines = {}
	for i, arg in ipairs(self.args) do
		arglines[i] = arg:defstring(lp)
	end
	return '('..table.concat(arglines, ', ')..')'
end

function farglist:compile_luai(lp)
	local arglines = {}
	for i, arg in ipairs(self.args) do
		arglines[i] = arg.innername
	end
	return '('..table.concat(arglines, ', ')..')'
end
