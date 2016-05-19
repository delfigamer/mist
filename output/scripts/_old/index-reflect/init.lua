local modname = ...
module(modname, package.seeall)
local ffi = require('ffi')
local host = require('host')
local uml = require('host.methodlist').utils
local window = require('host.window')

local utf8 = uml.encoding_getencoding(0)

function register()
	print('>>')
	while true do
		local ch = host.getchar()
		if ch == '' then
			break
		end
		local i = ffi.new('uint32_t[1]')
		local s = ffi.new('size_t[1]')
		if utf8.decode(ch, i, 0, s) then
			host.write(string.format('[U+%.4x]', i[0]))
		else
			host.write('[]')
		end
	end
	print('<<')
	window:finish()
end

function unregister()
end
