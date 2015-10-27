local modname = ...
local convolute = require('img.convolute')
local frame = require('img.frame')
local gausskernel = require('img.gausskernel')
local import = require('img.import')

local function bloomgen(it)
	local radius = it[2] or 10
	local gauss_x, gauss_y = gausskernel{radius}
	local sc = convolute{it[1], gauss_x, gauss_y}
	return frame{sc, radius, radius, -radius, -radius}
end

package.modtable(modname, bloomgen)
