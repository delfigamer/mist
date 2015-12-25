local modname = ...
local tbase = package.relrequire(modname, 1, 'base')
local tr = tbase:module(modname)
local token = package.relrequire(modname, 2, 'token')

tr.fields = [[
	uint32_t code;
	uint32_t ra;
]]

function tr:create(ct, ra)
	return self:new(ct.code, ra)
end

function tr.instmeta:__tostring()
	local name = token.codemap[self.code].name
	if name == 'v_table' then
		return string.format('\tlocal r%i = {',
			self.ra)
	else
		return string.format('%16s r%i',
			name, self.ra)
	end
end
