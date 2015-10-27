local modname = ...
local node = require('index-img.node')
local add = node:module(modname)
local fmat = require('index-img.fmat')

function add:process()
	local ifmat = assert(self.input0).result
	local afmat = assert(self.pintable[self.conf.operand]).result
	local ofmat = fmat:create(ifmat.header)
	local xoffset = self.conf.xoffset or 0
	local yoffset = self.conf.yoffset or 0
	local aoffset = self.conf.aoffset or 0
	local afactor = self.conf.afactor or 1
	local defvalue = self.conf.defvalue or {}
	for x = 0, ofmat.header.width - 1 do
		for y = 0, ofmat.header.height - 1 do
			for p = 0, ofmat.header.planes - 1 do
				ofmat.data[y][x][p] = ifmat.data[y][x][p] +
					(afmat:get(x + xoffset, y + yoffset, p) or defvalue[p] or 0)
						* afactor + aoffset
			end
		end
	end
	self.result = ofmat
end
