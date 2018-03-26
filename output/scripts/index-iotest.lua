local modname = ...
local index = package.modtable(modname)
local ffi = require('ffi')
local filestorage = require('host.filestorage')
local invoke = require('base.invoke')

local function getfilecontent(path)
	local storage = filestorage:create(path, 0)
	local offset = 0ull
	local parts = {}
	while true do
		local task, ptr, len = storage:map(offset, -1, true, false)
		if len == 0 then
			break
		end
		table.append(parts, ffi.string(ptr, len))
		offset = offset + len
		task:release()
	end
	storage:close()
	return table.concat(parts)
end

local function main()
	local cont = getfilecontent('build/l-win64-debug/client-main/methodlist.lua')
	print(string.format('%q', cont))
end

invoke(main)
