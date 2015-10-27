local modname = ...
local node = require('index-img.node')
local convolute = node:module(modname)
local fmat = require('index-img.fmat')

local function convalue(sfmat, valget, defvalue, plane, x, y, kfmat)
	local value = 0
	for dx = 0, kfmat.header.width-1 do
		for dy = 0, kfmat.header.height-1 do
			value = value + (valget(sfmat, x + dx, y + dy, plane) or defvalue) * kfmat.data[dy][dx][0]
		end
	end
	return value
end

function convolute:process()
	local ifmat = assert(self.input0).result
	local kfmat = assert(self.pintable[self.conf.kernel]).result
	local width, height
	if self.conf.extend then
		width = ifmat.header.width + kfmat.header.width - 1
		height = ifmat.header.height + kfmat.header.height - 1
	else
		width = ifmat.header.width
		height = ifmat.header.height
	end
	local ofmat = fmat:spawn(width, height, ifmat.header.planes)
	local xoffset = self.conf.xoffset
	local yoffset = self.conf.yoffset
	local valget
	local defvalue = self.conf.defvalue or {}
	if self.conf.wrap then
		xoffset = xoffset or (- bit.rshift(kfmat.header.width, 1))
		yoffset = yoffset or (- bit.rshift(kfmat.header.height, 1))
		valget = fmat.get_xywrap
	elseif self.conf.extend then
		xoffset = xoffset or (1 - kfmat.header.width)
		yoffset = yoffset or (1 - kfmat.header.height)
		valget = fmat.get
	else
		xoffset = xoffset or (- bit.rshift(kfmat.header.width, 1))
		yoffset = yoffset or (- bit.rshift(kfmat.header.height, 1))
		valget = self.conf.clip and fmat.get or fmat.get_xyclamp
	end
	for x = 0, ofmat.header.width - 1 do
		for y = 0, ofmat.header.height - 1 do
			for p = 0, ofmat.header.planes - 1 do
				ofmat.data[y][x][p] = convalue(ifmat, valget, defvalue[p] or 0, p, x + xoffset, y + yoffset, kfmat)
			end
		end
	end
	self.result = ofmat
end
