local modname = ...
local index = package.modtable(modname)
local ffi = require('ffi')
local databuffer = require('databuffer')
local filestorage = require('rsbin.filestorage')
local fileopenmode = require('rsbin.fileopenmode')
local storagestream = require('rsbin.storagestream')
local pngreader = require('rsbin.pngreader')
local pngwriter = require('rsbin.pngwriter')
local bitmapformat = require('rsbin.bitmapformat')
local invoke = require('base.invoke')

local function getfilecontent(path)
	local storage = filestorage:create(path, fileopenmode.read)
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

local function readpng(path, format)
	local storage = filestorage:create(path, fileopenmode.read)
	local stream = storagestream:create(storage, true, false, 0)
	local reader = pngreader:create(stream, bitmapformat[format])
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

local function writepng(path, format, width, height, pixels)
	local storage = filestorage:create(path, fileopenmode.create)
	local stream = storagestream:create(storage, false, true, 0)
	local writer = pngwriter:create(
		stream, bitmapformat[format], width, height, pixels)
	while not writer:poll() do
		coroutine.yield()
	end
	writer:release()
	stream:release()
	storage:close()
	storage:release()
end

local function main_streamtest()
	local cont = getfilecontent('build/l-win64-debug/client-main/event.r')
	print(string.format('%s', cont))
end

local function main_pngreadertest()
	local bitmap = readpng('assetsources/pixels.png', 'rgba8')
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

local function main_pngwritertest()
	local width = 256
	local height = 128
	local pixels = databuffer:create(4*width*height, 4*width*height, nil)
	local data = pixels:getdata()
	for y = 0, height-1 do
		local row = data + 4 * width * y
		for x = 0, width-1 do
			local pixel = row + 4 * x
			pixel[0] = x
			pixel[1] = 2*y
			pixel[2] = 0
			pixel[3] = 255
		end
	end
	writepng('local/pngtest.png', 'rgba8', width, height, pixels)
end

local function main_png16readertest()
	local bitmap = readpng('local/png16test.png', 'rgba16')
	local data = ffi.cast('uint16_t*', bitmap.pixels:getdata())
	for ch = 0, 3 do
		for y = 0, bitmap.height-1 do
			local row = data + 4 * bitmap.width * y + ch
			local str = {}
			for x = 0, bitmap.width-1 do
				local value = row[4 * x]
				table.append(str, string.format('%.4x', value))
			end
			print(table.concat(str, ' '))
		end
		print()
	end
end

local function main_png16writertest()
	local width = 8
	local height = 16
	local pixels = databuffer:create(8*width*height, 8*width*height, nil)
	local data = ffi.cast('uint16_t*', pixels:getdata())
	for y = 0, height-1 do
		local row = data + 4 * width * y
		for x = 0, width-1 do
			local pixel = row + 4 * x
			pixel[0] = 0x80*x
			pixel[1] = 0x40*y
			pixel[2] = 0
			pixel[3] = 0xffff
		end
	end
	writepng('local/png16test.png', 'rgba16', width, height, pixels)
end

invoke(main_png16readertest)
