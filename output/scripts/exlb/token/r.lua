local modname = ...
local tbase = require('exlb.token.base')
local tr = tbase:module(modname)
local token = require('exlb.token')

tr.fields = [[
	uint32_t code;
	uint32_t ra;
]]

function tr:create(ct, ra)
	return self:new(ct.code, ra)
end

function tr.instmeta:__tostring()
	local name = token.codemap[self.code].name
	if name == 'v_function' then
		return string.format('\tlocal r%i = function(',
			self.ra)
	else
		return string.format('%16s r%i',
			name, self.ra)
	end
end
