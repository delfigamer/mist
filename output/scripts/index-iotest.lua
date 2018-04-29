local modname = ...
local index = package.modtable(modname)
local ffi = require('ffi')
local filestorage = require('rsbin.filestorage')
local storagestream = require('rsbin.storagestream')
local pngreader = require('rsbin.pngreader')
local bitmapformat = require('rsbin.bitmapformat')
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
	storage:release()
	return table.concat(parts)
end

local function getpngbitmap(path)
	local storage = filestorage:create(path, 0)
	local stream = storagestream:create(storage, true, false, 0)
	local reader = pngreader:create(bitmapformat.rgba8, stream)
	while not reader:poll() do
		coroutine.yield()
	end
	local bitmap = {
		pixels = reader:getpixels(),
		width = reader:getwidth(),
		height = reader:getheight()}
	reader:release()
	stream:release()
	storage:close()
	storage:release()
	return bitmap
end

local function main()
	-- local cont = getfilecontent('build/l-win64-debug/client-main/event.r')
	-- print(string.format('%s', cont))
	local bitmap = getpngbitmap('assetsources/pixels.png')
	local data = bitmap.pixels:getdata()
	for ch = 0, 3 do
		for y = 0, bitmap.height-1 do
			local row = data + 4 * bitmap.width * y + ch
			local str = {}
			for x = 0, bitmap.width-1 do
				local value = row[4 * x]
				table.append(str, string.format('%.2x', value))
			end
			print(table.concat(str, ' '))
		end
		print()
	end
end

invoke(main)
