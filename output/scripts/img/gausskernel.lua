local modname = ...
local generate = require('img.generate')

local function gauss_gen(dev)
	local ef = -0.5/(dev*dev)
	local vf = 1/(math.sqrt(2*math.pi)*dev)
	return function(x)
		return vf * math.exp(x*x*ef)
	end
end

local function gausskernel(it)
	local radius = it[1] or 1
	local dev = it[2] or radius/2
	local gaussf = gauss_gen(dev)
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
	return gauss_x, gauss_y
end

package.modtable(modname, gausskernel)
