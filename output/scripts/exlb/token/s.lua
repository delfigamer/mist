local modname = ...
local tbase = require('exlb.token.base')
local ts = tbase:module(modname)
local ffi = require('ffi')
local token = require('exlb.token')

ts.fields = [[
	uint32_t code;
	uint32_t length;
	char data[?];
]]

function ts:create(ct, data)
	return self:new(#data, ct.code, #data, data)
end

function ts.instmeta:__tostring()
	local name = token.codemap[self.code].name
	local str = ffi.string(self.data, self.length)
	if name == 'd_filename' then
		return string.format('--[[%s]]',
			str)
	else
		return string.format('%16s %q',
			name, str)
	end
end
