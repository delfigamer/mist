local modname = ...
local tbase = package.relrequire(modname, 1, 'base')
local tr = tbase:module(modname)
local ffi = require('ffi')
local token = package.relrequire(modname, 2, 'token')

tr.fields = [[
	uint32_t code;
	uint32_t ra;
	uint32_t length_b;
	uint32_t length_c;
	uint32_t r[?];
]]

function tr:create(ct, ra, vrb, vrc)
	local inst = self:new(#vrb + #vrc, ct.code, ra, #vrb, #vrc)
	for i = 1, #vrb do
		inst.r[i-1] = vrb[i]
	end
	local offset = (#vrb - 1)
	for i = 1, #vrc do
		inst.r[i + offset] = vrc[i]
	end
	return inst
end

function tr.instmeta:__tostring()
	local name = token.codemap[self.code].name
	local rbstr = {}
	for i = 1, self.length_b do
		rbstr[i] = string.format('r%i', self.r[i-1])
	end
	rbstr = table.concat(rbstr, ', ')
	local rcstr = {}
	for i = 1, self.length_c do
		rcstr[i] = string.format('r%i', self.r[i+self.length_b-1])
	end
	rcstr = table.concat(rcstr, ', ')
	if name == 'a_call' then
		if self.length_b > 0 then
			return string.format('\tlocal %s = r%i(%s)',
				rbstr, self.ra, rcstr)
		else
			return string.format('\tr%i(%s)',
				self.ra, rcstr)
		end
	else
		return string.format('%16s r%i;%s;%s',
			name, self.ra, rbstr, rcstr)
	end
end
