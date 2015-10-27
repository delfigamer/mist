local mainconf = require('main')
local minv = mainconf.minv or 0
local maxv = mainconf.maxv or 1
local offset = minv
local factor = maxv - minv
local gamma = mainconf.gamma or 2.2

local function gauss_gen(dev)
	local ef = -0.5/(dev*dev)
	local vf = 1/(math.sqrt(2*math.pi)*dev)
	return function(x)
		return vf * math.exp(x*x*ef)
	end
end

local dev = mainconf.dev or 8

local gaussf = gauss_gen(dev)

return {
	{
		{ 'generate',
			width = dev*4+1,
			height = 1,
			planes = 1,
			genf = function(x, y)
				return gaussf(x - dev*2)
			end,
		},
		pin = 'gauss_x',
	},
	{
		{ 'generate',
			width = 1,
			height = dev*4+1,
			planes = 1,
			genf = function(x, y)
				return gaussf(y - dev*2)
			end,
		},
		pin = 'gauss_y',
	},
	{
		{ 'import',
			file = mainconf.input or 'input.png',
			offset = offset,
			factor = factor,
			gamma = gamma,
		},
		pin = 'source',
	},
	{
		[0] = 'source',
		{ 'convolute',
			kernel = 'gauss_x',
		},
		{ 'convolute',
			kernel = 'gauss_y',
		},
		{ 'save',
			file = mainconf.output or 'output.fmat',
		},
	},
}
