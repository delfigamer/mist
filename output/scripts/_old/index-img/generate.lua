local modname = ...
local node = require('index-img.node')
local generate = node:module(modname)
local bitmap = require('index-img.bitmap')
local fmat = require('index-img.fmat')

function generate:process()
	local width = self.conf.width or 1
	local height = self.conf.height or 1
	local planes = self.conf.planes or 1
	local genf = assert(self.conf.genf)
	local ofmat = fmat:spawn(width, height, planes)
	for x = 0, width - 1 do
		for y = 0, height - 1 do
				ofmat.data[y][x] = ofmat.celltype(genf(x, y))
		end
	end
	self.result = ofmat
end
