local modname = ...
local tbase = require('exlb.token.base')
local ts = tbase:module(modname)
local ffi = require('ffi')
local token = require('exlb.token')

ts.fields = [[
	uint32_t code;
	uint32_t ra;
	uint32_t length;
	char data[?];
]]

function ts:create(ct, ra, data)
	return self:new(#data, ct.code, ra, #data, data)
end

function ts.instmeta:__tostring()
	local name = token.codemap[self.code].name
	local str = ffi.string(self.data, self.length)
	if name == 'v_string' then
		return string.format('\tlocal r%i = %q',
			ra, str)
	else
		return string.format('%16s r%i, %q',
			name, ra, str)
	end
end
