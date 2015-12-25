local modname = ...
local tbase = package.relrequire(modname, 1, 'base')
local tr = tbase:module(modname)
local ffi = require('ffi')
local token = package.relrequire(modname, 2, 'token')

tr.fields = [[
	uint32_t code;
	uint32_t length;
	uint32_t r[?];
]]

function tr:create(ct, r)
	local inst = self:new(#r, ct.code, #r)
	for i = 1, #r do
		inst.r[i-1] = r[i]
	end
	return inst
end

function tr.instmeta:__tostring()
	local name = token.codemap[self.code].name
	local rstr = {}
	for i = 1, self.length do
		rstr[i] = string.format('r%i', self.r[i-1])
	end
	rstr = table.concat(rstr, ', ')
	if name == 'a_return' then
		return string.format('\tdo return %s end',
			rstr)
	else
		return string.format('%16s %s',
			name, str)
	end
end
