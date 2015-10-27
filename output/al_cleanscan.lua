local mainconf = require('main')

local function gauss_gen(dev)
	local ef = -0.5/(dev*dev)
	local vf = 1/(math.sqrt(2*math.pi)*dev)
	return function(x)
		return vf * math.exp(x*x*ef)
	end
end

local gauss5 = gauss_gen(5)

return {
	{
		{ 'generate',
			width = 21,
			height = 1,
			planes = 1,
			genf = function(x, y)
				return gauss5(x - 10)
			end,
		},
		pin = 'gauss_x',
	},
	{
		{ 'generate',
			width = 1,
			height = 21,
			planes = 1,
			genf = function(x, y)
				return gauss5(y - 10)
			end,
		},
		pin = 'gauss_y',
	},
	{
		{ 'import',
			file = mainconf.input or 'input.png',
			gray = true,
		},
		pin = 'source',
	},
	{
		[0] = 'source',
		{ 'locallims',
			radius = 5,
		},
		{ 'convolute',
			kernel = 'gauss_x',
		},
		{ 'convolute',
			kernel = 'gauss_y',
		},
		pin = 'lmax',
	},
	{
		[0] = 'source',
		{ 'divide',
			operand = 'lmax',
		},
		{ 'save',
			file = mainconf.output or 'output.fmat',
		},
	},
}
