local modname = ...
local node = require('index-img.node')
local export = node:module(modname)
local bitmap = require('index-img.bitmap')
local fmat = require('index-img.fmat')

local function clampf(c)
	if c > 1 then
		return 1
	elseif c < 0 then
		return 0
	else
		return c
	end
end

local function f32tou8_gamma(c, offset, factor, invgamma)
	return math.floor(math.pow(clampf((c - offset) / factor), invgamma) * 255 + 0.5)
end

function export:process()
	local ifmat = assert(self.input0).result
	local obitmap = bitmap:create(ifmat.header.width, ifmat.header.height)
	local invgamma = 1 / (self.conf.gamma or 2.2)
	local offset = self.conf.offset or 0
	local factor = self.conf.factor or 1
	if ifmat.header.planes == 1 then
		for x = 0, ifmat.header.width - 1 do
			for y = 0, ifmat.header.height - 1 do
				local value = f32tou8_gamma(ifmat.data[y][x][0], offset, factor, invgamma)
				obitmap.data[y][x].r = value
				obitmap.data[y][x].g = value
				obitmap.data[y][x].b = value
				obitmap.data[y][x].a = 255
			end
		end
	elseif ifmat.header.planes == 3 then
		for x = 0, ifmat.header.width - 1 do
			for y = 0, ifmat.header.height - 1 do
				obitmap.data[y][x].r = f32tou8_gamma(ifmat.data[y][x][0], offset, factor, invgamma)
				obitmap.data[y][x].g = f32tou8_gamma(ifmat.data[y][x][1], offset, factor, invgamma)
				obitmap.data[y][x].b = f32tou8_gamma(ifmat.data[y][x][2], offset, factor, invgamma)
				obitmap.data[y][x].a = 255
			end
		end
	else
		error('unsupported number of planes')
	end
	obitmap:writepng(self.conf.file or 'output.png')
	self.result = ifmat
end
