local modname = ...
local tbase = require('exlb.token.base')
local tr = tbase:module(modname)
local token = require('exlb.token')

tr.fields = [[
	uint32_t code;
	uint32_t ra;
	uint32_t l_depth;
	uint32_t length;
	uint32_t l_name[?];
]]

function tr:create(ct, ra, ldepth, lname)
	return self:new(#lname, ct.code, ra, ldepth, #lname, lname)
end

function tr.instmeta:__tostring()
	local name = token.codemap[self.code].name
	local lstr = {}
	for i = 1, self.length do
		lstr[i] = string.format('d%ii%i', self.l_depth, self.l_name[i-1])
	end
	lstr = table.concat(lstr, ', ')
	if name == 'v_function' then
		return string.format('\tlocal r%i = function(%s)',
			self.ra, lstr)
	elseif self.length > 0 then
		return string.format('%16s r%i, %s',
			name, self.ra, lstr)
	else
		return string.format('%16s r%i',
			name, self.ra)
	end
end
