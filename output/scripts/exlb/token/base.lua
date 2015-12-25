local modname = ...
local ffipure = require('base.ffipure')
local tbase = ffipure:module(modname)
local token = package.relrequire(modname, 2, 'token')

tbase.fields = [[
	uint32_t code;
]]

function tbase:create(ct)
	return self:new(ct.code)
end

function tbase.instmeta:__tostring()
	local name = token.codemap[self.code].name
	if name == 'v_function_end' then
		return '\t\tend --[[function]]'
	elseif name == 'v_table_end' then
		return '\t\t}'
	elseif name == 'd_filename_end' then
		return '--[[file end]]'
	else
		return string.format('%16s',
			name)
	end
end
