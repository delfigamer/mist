local modname = ...
local node = require('index-img.node')
local integrate = node:module(modname)
local fmat = require('index-img.fmat')

function integrate:process()
	local ifmat = assert(self.input0).result
	local ofmat = fmat:create(ifmat.header)
	for p = 0, ofmat.header.planes - 1 do
		ofmat.data[0][0][p] = ifmat.data[0][0][p]
	end
	for y = 1, ofmat.header.height - 1 do
		for p = 0, ofmat.header.planes - 1 do
			ofmat.data[y][0][p] = ofmat.data[y-1][0][p] + ifmat.data[y][0][p]
		end
	end
	for x = 1, ofmat.header.width - 1 do
		for p = 0, ofmat.header.planes - 1 do
			ofmat.data[0][x][p] = ofmat.data[0][x-1][p] + ifmat.data[0][x][p]
		end
		for y = 1, ofmat.header.height - 1 do
			for p = 0, ofmat.header.planes - 1 do
				ofmat.data[y][x][p] =
					ofmat.data[y-1][x][p] + ofmat.data[y][x-1][p] -
					ofmat.data[y-1][x-1][p] + ifmat.data[y][x][p]
			end
		end
	end
	self.result = ofmat
end
