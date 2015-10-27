local modname = ...
local convolute = require('img.convolute')
local add = require('img.add')
local export = require('img.export')
local frame = require('img.frame')
local gausskernel = require('img.gausskernel')
local import = require('img.import')

local function cleanscan(it)
	local radius = it.radius or 5
	local gaussf = gauss_gen(radius/1.5)
	local gauss_x = generate{
		width = radius*2+1,
		height = 1,
		planes = 1,
		function(x, y)
			return gaussf(x - radius)
		end,
	}
	local gauss_y = generate{
		width = 1,
		height = radius*2+1,
		planes = 1,
		function(x, y)
			return gaussf(y - radius)
		end,
	}
	local source = import{
		it[1] or 'input.png',
		gray = true,
	}
	local lmax = locallims{source, radius = radius}
	lmax = convolute{lmax, gauss_x}
	lmax = convolute{lmax, gauss_y}
	lmax = frame{lmax, radius, radius, -radius, -radius}
	local norm = divide{source, lmax}
	return norm
end

package.modtable(modname, cleanscan)
