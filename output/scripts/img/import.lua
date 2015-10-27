local modname = ...
local ffi = require('ffi')
local fileio = require('rs.fileio')
local fmat = require('img.fmat')
local invoke = require('base.invoke')
local iostream = require('rs.iostream')
local pngptype = require('rs.pngptype')

local function import_file(path)
	local io = fileio:create(path, 'r')
	local stream = iostream:create(io)
	local bm = pngptype:read(stream)
	stream:release()
	io:release()
	return bm
end

local function u8tof32_gamma(c, offset, factor, gamma)
	return math.pow(c / 255, gamma) * factor + offset
end

local function import(it)
	local bm = invoke(import_file, it[1])
	local result = fmat:spawn(bm.width, bm.height, it.gray and 1 or 3)
	local gamma = it.gamma or 2.2
	local offset = it.offset or 0
	local factor = it.factor or 1
	local ptr = ffi.cast('uint8_t*', bm.db:getdata())
	if it.gray then
		for y = 0, bm.height - 1 do
			local row = ptr + y * bm.width * 4
			for x = 0, bm.width - 1 do
				local pixel = row + x * 4
				result.data[y][x][0] =
					(math.pow(pixel[0]/255, gamma) * factor + offset) * 0.2126 +
					(math.pow(pixel[1]/255, gamma) * factor + offset) * 0.7152 +
					(math.pow(pixel[2]/255, gamma) * factor + offset) * 0.0722
			end
		end
	else
		for y = 0, bm.height - 1 do
			local row = ptr + y * bm.width * 4
			for x = 0, bm.width - 1 do
				local pixel = row + x * 4
				result.data[y][x][0] =
					math.pow(pixel[0]/255, gamma) * factor + offset
				result.data[y][x][1] =
					math.pow(pixel[1]/255, gamma) * factor + offset
				result.data[y][x][2] =
					math.pow(pixel[2]/255, gamma) * factor + offset
			end
		end
	end
	return result
end

package.modtable(modname, import)
