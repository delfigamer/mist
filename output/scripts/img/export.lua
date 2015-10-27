local modname = ...
local databuffer = require('host.databuffer')
local ffi = require('ffi')
local fileio = require('rs.fileio')
local fmat = require('img.fmat')
local invoke = require('base.invoke')
local iostream = require('rs.iostream')
local pngptype = require('rs.pngptype')

local function export_file(path, bm)
	local io = fileio:create(path, 'w')
	local stream = iostream:create(io)
	pngptype:write(bm, stream)
	stream:release()
	io:release()
end

local function clampf(c)
	return (math.abs(c) - math.abs(c-1) + 1) * 0.5
end

local function f32tou8_gamma(c, offset, factor, invgamma)
	return math.floor(math.pow(clampf(c * factor + offset), invgamma) * 255 + 0.5)
end

local function export(it)
	local ifmat = it[2]
	local width = ifmat.header.width
	local height = ifmat.header.height
	local len = width * height * 4
	local bm = {
		width = width,
		height = height,
		db = databuffer:create(len, len),
	}
	local ptr = ffi.cast('uint8_t*', bm.db:getdata())
	local invgamma = it.invgamma or 1/2.2
	local offset = it.offset or 0
	local factor = it.factor or 1
	if ifmat.header.planes == 1 then
		for y = 0, height - 1 do
			local row = ptr + y * width * 4
			for x = 0, width - 1 do
				local pixel = row + x * 4
				local value = f32tou8_gamma(ifmat.data[y][x][0], offset, factor, invgamma)
				pixel[0] = value
				pixel[1] = value
				pixel[2] = value
				pixel[3] = 255
			end
		end
	elseif ifmat.header.planes == 3 then
		for y = 0, height - 1 do
			local row = ptr + y * width * 4
			for x = 0, width - 1 do
				local pixel = row + x * 4
				pixel[0] = f32tou8_gamma(ifmat.data[y][x][0], offset, factor, invgamma)
				pixel[1] = f32tou8_gamma(ifmat.data[y][x][1], offset, factor, invgamma)
				pixel[2] = f32tou8_gamma(ifmat.data[y][x][2], offset, factor, invgamma)
				pixel[3] = 255
			end
		end
	else
		error('unsupported number of planes')
	end
	invoke(export_file, it[1], bm)
end

package.modtable(modname, export)
