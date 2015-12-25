local modname = ...
local tbase = package.relrequire(modname, 1, 'base')
local tr = tbase:module(modname)
local token = package.relrequire(modname, 2, 'token')

tr.fields = [[
	uint32_t code;
	uint32_t ra;
	uint32_t rb;
	uint32_t rc;
]]

function tr:create(ct, ra, rb, rc)
	return self:new(ct.code, ra, rb, rc)
end

local sign = {
	a_add = '+',
	a_sub = '-',
	a_mul = '*',
	a_div = '/',
	a_concat = '..',
}

function tr.instmeta:__tostring()
	local name = token.codemap[self.code].name
	if sign[name] then
		return string.format('\tlocal r%i = r%i %s r%i',
			self.ra, self.rb, sign[name], self.rc)
	else
		return string.format('%16s r%i, r%i, r%i',
			name, self.ra, self.rb, self.rc)
	end
end
