local modname = ...
local tbase = require('exlb.token.base')
local tr = tbase:module(modname)
local token = require('exlb.token')

tr.fields = [[
	uint32_t code;
	uint32_t ra;
	uint32_t l_depth;
	uint32_t l_name;
]]

function tr:create(ct, ra, id)
	return self:new(ct.code, ra, id.depth, id.name)
end

function tr.instmeta:__tostring()
	local name = token.codemap[self.code].name
	if name == 'a_getl' then
		return string.format('\tlocal r%i = d%ii%i',
			self.ra, self.l_depth, self.l_name)
	else
		return string.format('%16s r%i, d%ii%i',
			name, self.ra, self.l_depth, self.l_name)
	end
end
