local modname = ...
local tbase = require('exlb.token.base')
local tr = tbase:module(modname)
local token = require('exlb.token')

tr.fields = [[
	uint32_t code;
	uint32_t l_depth;
	uint32_t l_name;
	uint32_t ra;
]]

function tr:create(ct, id, ra)
	return self:new(ct.code, id.depth, id.name, ra)
end

function tr.instmeta:__tostring()
	local name = token.codemap[self.code].name
	if name == 'a_setl' then
		return (string.format('\td%ii%i = r%i',
			self.l_depth, self.l_name, self.ra))
	else
		return (string.format('%16s d%ii%i, r%i',
			name, self.l_depth, self.l_name, self.ra))
	end
end
