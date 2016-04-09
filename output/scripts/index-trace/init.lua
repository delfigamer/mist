local modname = ...
local index = package.modtable(modname)
local ffi = require('ffi')
local img = {
	export = require('img.export'),
	fmat = require('img.fmat'),
}

local intersection = require(modname, 0, 'intersection')
local triangle = require(modname, 0, 'triangle')
local vector = require(modname, 0, 'vector')

local size = {256, 256}

local ttri = triangle:create{
	{-1, -1,  1},
	{ 1, -1, -1},
	{-1,  1, -1},
}

local function pixelvalue(x, y)
	local ro = vector:create(x, y, 1)
	local rd = vector:create(0, 0, -2)
	local int = intersection:create(ttri, ro, rd)
	local wa = int.weight_a
	local wb = int.weight_b
	local wc = int.weight_c
	if wa < 0 or wb < 0 or wc < 0 then
		return ffi.new('float[3]', 0, 0, 0)
	else
		return ffi.new('float[3]', wa, wb, wc)
	end
end

local fmat = img.fmat:spawn(size[1], size[2], 3)
for x = 0, size[1]-1 do
	for y = 0, size[2]-1 do
		local rx = x*(2/(size[1]-1))-1
		local ry = y*(2/(size[2]-1))-1
		fmat.data[y][x] = pixelvalue(rx, ry)
	end
end

img.export{'output.png', fmat}
