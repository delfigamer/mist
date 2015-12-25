local modname = ...
local tbase = package.relrequire(modname, 1, 'base')
local tr = tbase:module(modname)
local token = package.relrequire(modname, 2, 'token')

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
	if name == 'a_createl' then
		return string.format('\tlocal d%ii%i',
			self.l_depth, self.l_name)
	else
		return string.format('%16s d%ii%i',
			name, self.l_depth, self.l_name)
	end
end
