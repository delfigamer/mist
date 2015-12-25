local modname = ...
local tbase = package.relrequire(modname, 1, 'base')
local tr = tbase:module(modname)
local token = package.relrequire(modname, 2, 'token')

tr.fields = [[
	uint32_t code;
	uint32_t ia;
	uint32_t ib;
]]

function tr:create(ct, ia, ib)
	return self:new(ct.code, ia, ib)
end

function tr.instmeta:__tostring()
	local name = token.codemap[self.code].name
	if name == 'd_filepos' then
		return string.format('--[[%i:%i]]',
			self.ia, self.ib)
	else
		return string.format('%16s %i:%i',
			name, self.ia, self.ib)
	end
end
