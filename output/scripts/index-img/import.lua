local modname = ...
local node = require('index-img.node')
local import = node:module(modname)
local bitmap = require('index-img.bitmap')
local fmat = require('index-img.fmat')

local function u8tof32_gamma(c, offset, factor, gamma)
	return math.pow(c / 255, gamma) * factor + offset
end

function import:process()
	local ibitmap = bitmap:readpng(self.conf.file or 'input.png')
	local result = fmat:spawn(ibitmap.width, ibitmap.height, self.conf.gray and 1 or 3)
	local gamma = self.conf.gamma or 2.2
	local offset = self.conf.offset or 0
	local factor = self.conf.factor or 1
	if self.conf.gray then
		for x = 0, ibitmap.width - 1 do
			for y = 0, ibitmap.height - 1 do
				result.data[y][x][0] =
					u8tof32_gamma(ibitmap.data[y][x].r, offset, factor, gamma) * 0.2126 +
					u8tof32_gamma(ibitmap.data[y][x].g, offset, factor, gamma) * 0.7152 +
					u8tof32_gamma(ibitmap.data[y][x].b, offset, factor, gamma) * 0.0722
			end
		end
	else
		for x = 0, ibitmap.width - 1 do
			for y = 0, ibitmap.height - 1 do
				result.data[y][x][0] = u8tof32_gamma(ibitmap.data[y][x].r, offset, factor, gamma)
				result.data[y][x][1] = u8tof32_gamma(ibitmap.data[y][x].g, offset, factor, gamma)
				result.data[y][x][2] = u8tof32_gamma(ibitmap.data[y][x].b, offset, factor, gamma)
			end
		end
	end
	self.result = result
end
