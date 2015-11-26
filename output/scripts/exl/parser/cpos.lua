local modname = ...
local ffipure = require('base.ffipure')
local cpos = ffipure:module(modname)

cpos.fields = [[
	int row;
	int col;
]]

function cpos:create(...)
	return cpos:new(...)
end

function cpos.instmeta:__tostring()
	return string.format('[%i:%i]', self.row, self.col)
end
