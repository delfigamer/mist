local modname = ...
local index = package.modtable(modname)
local ffi = require('ffi')
local filestorage = require('rsbin.filestorage')
local storagestream = require('rsbin.storagestream')
local invoke = require('base.invoke')

local function getfilecontent(path)
	local storage = filestorage:create(path, 0)
	local stream = storagestream:create(storage, true, false, 0)
	local parts = {}
	while true do
		local task, ptr, len = stream:advance(-1)
		if len == 0 then
			break
		end
		table.append(parts, ffi.string(ptr, len))
		task:release()
	end
	stream:release()
	storage:close()
	return table.concat(parts)
end

local function main()
	local cont = getfilecontent('build/l-win64-debug/client-main/event.r')
	print(string.format('%s', cont))
end

invoke(main)
