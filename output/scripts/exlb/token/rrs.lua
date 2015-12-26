local modname = ...
local tbase = package.relrequire(modname, 1, 'base')
local ts = tbase:module(modname)
local ffi = require('ffi')
local token = package.relrequire(modname, 2, 'token')

ts.fields = [[
	uint32_t code;
	uint32_t ra;
	uint32_t rb;
	uint32_t length;
	char data[?];
]]

function ts:create(ct, ra, rb, data)
	local inst = self:new(#data, ct.code, ra, rb, #data)
	ffi.copy(inst.data, data, inst.length)
	return inst
end

function ts.instmeta:__tostring()
	local name = token.codemap[self.code].name
	local str = ffi.string(self.data, self.length)
	if name == 'v_class' then
		if self.rb == 0 then
			return string.format(
				'\tlocal r%i = require("base.object"):derive(nil, %q)',
				self.ra, str)
		else
			return string.format('\tlocal r%i = r%i:derive(nil, %q)',
				self.ra, self.rb, str)
		end
	else
		return string.format('%16s r%i, %q',
			name, self.ra, str)
	end
end
