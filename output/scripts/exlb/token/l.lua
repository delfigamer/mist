local modname = ...
local tbase = require('exlb.token.base')
local tr = tbase:module(modname)
local token = require('exlb.token')

tr.fields = [[
	uint32_t code;
	uint32_t l_depth;
	uint32_t l_name;
]]

function tr:create(ct, id)
	return self:new(ct.code, id.depth, id.name)
end

function tr.instmeta:__tostring()
	local name = token.codemap[self.code].name
	if name == 'v_function_argument' then
		return string.format('\t\t\td%ii%i,',
			self.l_depth, self.l_name)
	else
		return string.format('%16s d%ii%i',
			name, self.l_depth, self.l_name)
	end
end
