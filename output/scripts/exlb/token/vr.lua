local modname = ...
local tbase = require('exlb.token.base')
local tr = tbase:module(modname)
local ffi = require('ffi')
local token = require('exlb.token')

tr.fields = [[
	uint32_t code;
	uint32_t length;
	uint32_t r[?];
]]

function tr:create(ct, r)
	return self:new(#r, ct.code, #r, r)
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
