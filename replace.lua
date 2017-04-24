assert(loadfile('base.lua'))()
local env = require('env')
local path = ...
assert(path)

local repl = {
	{'#include <cinttypes>', ''},
	{'#include <cstdint>', ''},
}

env.makepath('build/')
local log = assert(io.open('build/rpending.lua', 'a'))
local fileintroduced = false

local row = 1
do
	local stream = assert(io.open(path, 'r'))
	while true do
		local line = stream:read('*l')
		if not line then
			break
		end
		local target = line
		for i, entry in ipairs(repl) do
			target = string.gsub(target, entry[1], entry[2])
		end
		if target ~= line then
			if not fileintroduced then
				log:write(string.format('{\n  path = %q,\n', path))
				fileintroduced = true
			end
			log:write(string.format('  [%5i] = {%q,\n', row, line))
			log:write(string.format('             %q},\n', target))
		end
		row = row + 1
	end
	stream:close()
end

if fileintroduced then
	log:write('},\n')
end
