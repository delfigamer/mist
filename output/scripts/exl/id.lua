local modname = ...
local ffipure = require('base.ffipure')
local id = ffipure:module(modname)

id.fields = [[
	int depth;
	int name;
]]

function id:init(depth, name)
	self.depth = depth
	self.name = name
end

local function idtostr(i)
	local s = ''
	repeat
		local ci = math.mod(i, 26)
		s = string.char(ci + string.byte('a'))
		i = math.floor(i / 26)
	until i <= 0
	return s
end

function id.instmeta:__tostring()
	return string.format('%i:%i', self.depth, self.name)
end
