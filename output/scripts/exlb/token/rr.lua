local modname = ...
local tbase = package.relrequire(modname, 1, 'base')
local tr = tbase:module(modname)
local token = package.relrequire(modname, 2, 'token')

tr.fields = [[
	uint32_t code;
	uint32_t ra;
	uint32_t rb;
]]

function tr:create(ct, ra, rb)
	return self:new(ct.code, ra, rb)
end

local sign = {
	a_negate = '-',
}

function tr.instmeta:__tostring()
	local name = token.codemap[self.code].name
	if sign[name] then
		return string.format('\tlocal r%i = %s r%i',
			self.ra, sign[name], self.rb)
	elseif name == 'a_instance' then
		return string.format('\tlocal r%i = r%i:new()',
			self.ra, self.rb)
	else
		return string.format('%16s r%i, r%i, r%i',
			name, self.ra, self.rb, self.rc)
	end
end
