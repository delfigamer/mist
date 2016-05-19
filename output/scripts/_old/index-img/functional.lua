local modname = ...
local node = require('index-img.node')
local functional = node:module(modname)
local fmat = require('index-img.fmat')
local utility = require('base.utility')

function functional:process()
	local ifmat = assert(self.input0).result
	local afmat
	if self.conf.operand then
		afmat = assert(self.pintable[self.conf.operand]).result
	end
	local planes = self.conf.planes or 1
	local ofmat = fmat:spawn(ifmat.header.width, ifmat.header.height, planes)
	local func = self.conf.func or utility.dummy
	for x = 0, ofmat.header.width - 1 do
		for y = 0, ofmat.header.height - 1 do
			func(ofmat.data[y][x], x, y, ifmat, afmat)
		end
	end
	self.result = ofmat
end
