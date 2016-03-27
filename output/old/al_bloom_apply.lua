local mainconf = require('main')
local minv = mainconf.minv or 0
local maxv = mainconf.maxv or 1
local offset = minv
local factor = maxv - minv
local gamma = mainconf.gamma or 2.2
local intensity = mainconf.intensity or 1

return {
	{
		{ 'import',
			file = mainconf.input or 'input.png',
		},
		pin = 'source',
	},
	{
		{ 'load',
			file = mainconf.input or 'output.fmat',
		},
		pin = 'bloom',
	},
	{
		[0] = 'source',
		{ 'add',
			operand = 'bloom',
			afactor = intensity,
		},
		{ 'export',
			offset = offset,
			factor = factor,
			gamma = gamma,
			file = mainconf.output or 'output.png',
		},
	},
}
