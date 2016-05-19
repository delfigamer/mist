local modname = ...
module(modname, package.seeall)
local mistsh = require('mistsh')

local source = io.open(_PATH .. 'testshader.mistsh', 'r'):read('*a'):gsub('\t', '   ')
local result = mistsh.translate(source)
print(result.vertex)
print('________\n')
print(result.fragment)
print('________\n')
print(string.format('%q', result.texnames))

if _PLATFORM == 'win' then
	io.open(_PATH .. 'testshader_vs.hlsl', 'w'):write(result.vertex)
	io.open(_PATH .. 'testshader_ps.hlsl', 'w'):write(result.fragment)
elseif _PLATFORM == 'android' then
	io.open(_PATH .. 'testshader_vs.vert', 'w'):write(result.vertex)
	io.open(_PATH .. 'testshader_ps.frag', 'w'):write(result.fragment)
end
