local mainconf = require('main')

return {
	{
		{ 'generate',
			width = 2,
			height = 2,
			planes = 1,
			genf = function(x, y)
				return 0.25
			end,
		},
		pin = 'box',
	},
	{
		{ 'import',
			file = mainconf.input or 'input.png',
		},
		pin = 'source',
	},
	{
		[0] = 'source',
		{ 'convolute',
			kernel = 'box',
			wrap = mainconf.wrap,
		},
		{ 'resample',
			widthfactor = 0.5,
			heightfactor = 0.5,
			wrap = mainconf.wrap,
		},
		pin = 'smooth',
	},
	{
		[0] = 'smooth',
		{ 'export',
			file = mainconf.output or 'output.png',
		},
	},
}
