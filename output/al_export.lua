local mainconf = require('main')
local minv = mainconf.minv or 0
local maxv = mainconf.maxv or 1
local offset = minv
local factor = maxv - minv
local gamma = mainconf.gamma or 2.2

return {
	{
		{ 'load',
			file = mainconf.input or 'output.fmat',
		},
		{ 'export',
			offset = offset,
			factor = factor,
			gamma = gamma,
			file = mainconf.output or 'output.png',
		},
	},
}
