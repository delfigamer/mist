local modname = ...
local index = package.modtable(modname)
local ffi = require('ffi')
local img = {
	export = require('img.export'),
	fmat = require('img.fmat'),
	import = require('img.import'),
}

local function pch(p)
	return p[0], p[1], p[2]
end

local function bilinear(v1, v2, v3, v4, wx, wy)
	local v12 = v1 + (v2-v1)*wx
	local v34 = v3 + (v4-v3)*wx
	return v12 + (v34-v12)*wy
end

local function median(a, b, c)
	return math.max(math.min(a, b), math.min(math.max(a, b), c))
end

local function dmap(f, nx, ny, c)
	local tx = math.fmod(nx,1) * f.header.width
	local ty = math.fmod(ny,1) * f.header.height
	local ix = math.floor(tx)
	local iy = math.floor(ty)
	local fx = tx - ix
	local fy = ty - iy
	local inx = ix + 1
	local iny = iy + 1
	if inx == f.header.width then
		inx = 0
	end
	if iny == f.header.height then
		iny = 0
	end
	local r1 = f.data[iy][ix][c]
	local r2 = f.data[iy][inx][c]
	local r3 = f.data[iny][ix][c]
	local r4 = f.data[iny][inx][c]
	local r = bilinear(r1, r2, r3, r4, fx, fy)
	return r*2-1
end

local function atest(v, min, max)
	if v >= min and v <= max then
		return 1
	else
		return 0
	end
end

local glyph = img.import{'a.png', gamma = 1}
local res = img.fmat:spawn(512, 512, 1)
for y = 0, 511 do
	for x = 0, 511 do
		res.data[y][x][0] = median(
			dmap(glyph, x/512, y/512, 0),
			dmap(glyph, x/512, y/512, 1),
			dmap(glyph, x/512, y/512, 2))
	end
end
img.export{'aa.png', res}
