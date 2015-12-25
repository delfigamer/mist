local modname = ...
local tbase = package.relrequire(modname, 1, 'base')
local ts = tbase:module(modname)
local ffi = require('ffi')
local token = package.relrequire(modname, 2, 'token')

ts.fields = [[
	uint32_t code;
	uint32_t ra;
	uint32_t length;
	char data[?];
]]

function ts:create(ct, ra, data)
	local inst = self:new(#data, ct.code, ra, #data)
	ffi.copy(inst.data, data, inst.length)
	return inst
end

function ts.instmeta:__tostring()
	local name = token.codemap[self.code].name
	local str = ffi.string(self.data, self.length)
	if name == 'v_string' then
		return string.format('\tlocal r%i = %q',
			self.ra, str)
	else
		return string.format('%16s r%i, %q',
			name, self.ra, str)
	end
end
