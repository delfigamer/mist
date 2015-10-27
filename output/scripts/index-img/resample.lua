local modname = ...
local node = require('index-img.node')
local resample = node:module(modname)
local fmat = require('index-img.fmat')
local utility = require('base.utility')

local function interpolate(a, b, t, wf)
	t = wf(t)
	return a * (1 - t) + b * t
end

local function sample(fmat, valget, nx, ny, p, defvalue, weightf)
	local bx, by = fmat.header.width * nx - 0.5, fmat.header.height * ny - 0.5
	local wx, wy = math.floor(bx), math.floor(by)
	local fx, fy = bx - wx, by - wy
	local s1 = valget(fmat, wx, wy, p) or defvalue[p] or 0
	local s2 = valget(fmat, wx+1, wy, p) or defvalue[p] or 0
	local s3 = valget(fmat, wx, wy+1, p) or defvalue[p] or 0
	local s4 = valget(fmat, wx+1, wy+1, p) or defvalue[p] or 0
	local sd, su
	sd, su = interpolate(s1, s2, fx, weightf), interpolate(s3, s4, fx, weightf)
	return interpolate(sd, su, fy, weightf)
end

function resample:process()
	local ifmat = assert(self.input0).result
	local width = self.conf.widthfactor
	if width then
		width = math.floor(ifmat.header.width * width)
	else
		width = self.conf.width or 1
	end
	local height = self.conf.heightfactor
	if height then
		height = math.floor(ifmat.header.height * height)
	else
		height = self.conf.height or 1
	end
	local ofmat = fmat:spawn(width, height, ifmat.header.planes)
	local weightf = self.conf.weightf or utility.operator.identity
	local valget = self.conf.wrap and fmat.get_xywrap or (
		self.conf.clip and fmat.get or fmat.get_xyclamp)
	local defvalue = self.conf.defvalue or {}
	for x = 0, ofmat.header.width - 1 do
		local nx = (x + 0.5) / ofmat.header.width
		for y = 0, ofmat.header.height - 1 do
			local ny = (y + 0.5) / ofmat.header.height
			for p = 0, ofmat.header.planes - 1 do
				ofmat.data[y][x][p] = sample(ifmat, valget, nx, ny, p, defvalue, weightf)
			end
		end
	end
	self.result = ofmat
end
