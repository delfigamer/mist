local modname = ...
local node = require('index-img.node')
local divide = node:module(modname)
local fmat = require('index-img.fmat')

function divide:process()
	local ifmat = assert(self.input0).result
	local afmat = assert(self.pintable[self.conf.operand]).result
	local ofmat = fmat:spawn(
		math.min(ifmat.header.width, afmat.header.width),
		math.min(ifmat.header.height, afmat.header.height),
		math.min(ifmat.header.planes, afmat.header.planes))
	local voffset = self.conf.voffset or 0
	local vfactor = self.conf.vfactor or 1
	for x = 0, ofmat.header.width - 1 do
		for y = 0, ofmat.header.height - 1 do
			for p = 0, ofmat.header.planes - 1 do
				local divisor = afmat.data[y][x][p] * vfactor + voffset
				ofmat.data[y][x][p] = ifmat.data[y][x][p] / divisor
			end
		end
	end
	self.result = ofmat
end
