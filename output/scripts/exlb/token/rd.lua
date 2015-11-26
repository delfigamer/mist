local modname = ...
local tbase = require('exlb.token.base')
local td = tbase:module(modname)
local token = require('exlb.token')

td.fields = [[
	uint32_t code;
	uint32_t ra;
	double value;
]]

function td:create(ct, ra, value)
	return self:new(ct.code, ra, value)
end

function td.instmeta:__tostring()
	local name = token.codemap[self.code].name
	if name == 'v_number' then
		return string.format('\tlocal r%i = %s',
			self.ra, self.value)
	else
		return string.format('%16s r%i, %s',
			name, self.ra, self.value)
	end
end
