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
	return math.floor(
		math.pow(clampf(c * factor + offset), invgamma) * 255 + 0.5)
end

local function f32tou24(c, offset, factor)
	local value = math.floor(clampf(c * factor + offset) * 0xffffff + 0.5)
	return
		bit.rshift(value, 16),
		bit.band(bit.rshift(value, 8), 0xff),
		bit.band(value, 0xff)
end

local function f32tou24log(c, offset, factor, power)
	return f32tou24(math.log(c * factor + offset, power), 0, 1)
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
	local power = it.power or math.exp(1)
	if ifmat.header.planes == 1 then
		for y = 0, height - 1 do
			local row = ptr + y * width * 4
			for x = 0, width - 1 do
				local pixel = row + x * 4
				if it.encode24 then
					local r, g, b = f32tou24(
						ifmat.data[y][x][0], offset, factor)
					pixel[0] = r
					pixel[1] = g
					pixel[2] = b
				elseif it.encode24log then
					local r, g, b = f32tou24log(
						ifmat.data[y][x][0], offset, factor, power)
					pixel[0] = r
					pixel[1] = g
					pixel[2] = b
				else
					local value = f32tou8_gamma(
						ifmat.data[y][x][0], offset, factor, invgamma)
					pixel[0] = value
					pixel[1] = value
					pixel[2] = value
				end
				pixel[3] = 255
			end
		end
	elseif ifmat.header.planes == 2 then
		for y = 0, height - 1 do
			local row = ptr + y * width * 4
			for x = 0, width - 1 do
				local pixel = row + x * 4
				pixel[0] =
					f32tou8_gamma(ifmat.data[y][x][0], offset, factor, invgamma)
				pixel[1] =
					f32tou8_gamma(ifmat.data[y][x][1], offset, factor, invgamma)
				pixel[2] = 0
				pixel[3] = 255
			end
		end
	elseif ifmat.header.planes == 3 then
		for y = 0, height - 1 do
			local row = ptr + y * width * 4
			for x = 0, width - 1 do
				local pixel = row + x * 4
				pixel[0] =
					f32tou8_gamma(ifmat.data[y][x][0], offset, factor, invgamma)
				pixel[1] =
					f32tou8_gamma(ifmat.data[y][x][1], offset, factor, invgamma)
				pixel[2] =
					f32tou8_gamma(ifmat.data[y][x][2], offset, factor, invgamma)
				pixel[3] = 255
			end
		end
	else
		error('unsupported number of planes')
	end
	invoke(export_file, it[1], bm)
end

package.modtable(modname, export)
