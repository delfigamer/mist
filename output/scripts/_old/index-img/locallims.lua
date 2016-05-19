local modname = ...
local node = require('index-img.node')
local locallims = node:module(modname)
local fmat = require('index-img.fmat')

local function gen_limvalue(min)
	local initlimit = min and math.huge or -math.huge
	local comparer
	if min then
		comparer = function(a, b) return a < b end
	else
		comparer = function(a, b) return a > b end
	end
	return function(sfmat, plane, x, y, xradius, yradius)
		local value = initlimit
		for dx = -xradius, xradius do
			for dy = -yradius, yradius do
				local sv = sfmat:get_xyclamp(x + dx, y + dy, plane)
				if comparer(sv, value) then
					value = sv
				end
			end
		end
		return value
	end
end

local maxvalue = gen_limvalue(false)
local minvalue = gen_limvalue(true)

function locallims:process()
	local ifmat = assert(self.input0).result
	local tfmat = fmat:create(ifmat.header)
	local ofmat = fmat:create(ifmat.header)
	local limvalue = self.conf.min and minvalue or maxvalue
	local radius = self.conf.radius or 1
	for x = 0, ofmat.header.width - 1 do
		for y = 0, ofmat.header.height - 1 do
			for p = 0, ofmat.header.planes - 1 do
				tfmat.data[y][x][p] = limvalue(ifmat, p, x, y, radius, 0)
			end
		end
	end
	for x = 0, ofmat.header.width - 1 do
		for y = 0, ofmat.header.height - 1 do
			for p = 0, ofmat.header.planes - 1 do
				ofmat.data[y][x][p] = limvalue(tfmat, p, x, y, 0, radius)
			end
		end
	end
	self.result = ofmat
end
