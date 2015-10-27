local mainconf = require('main')
local zscale = mainconf.zscale or 1
local noisescale = mainconf.noisescale or 0.01

return {
	{
		{ 'import',
			file = mainconf.input or 'input.png',
			gamma = 1,
			gray = true,
		},
		{ 'functional',
			planes = 3,
			func = function(target, x, y, ifmat)
				local h1 = ifmat:get_xywrap(x, y, 0) * zscale + math.random() * noisescale
				local h2 = ifmat:get_xywrap(x+1, y, 0) * zscale + math.random() * noisescale
				local h3 = ifmat:get_xywrap(x, y+1, 0) * zscale + math.random() * noisescale
				local h4 = ifmat:get_xywrap(x+1, y+1, 0) * zscale + math.random() * noisescale
				local dhdx = (h2 + h4 - h1 - h3) / 2
				local dhdy = (h3 + h4 - h1 - h2) / 2
				local nf = 1 / math.sqrt(1 + dhdx*dhdx + dhdy*dhdy)
				target[0] = -dhdx*nf
				target[1] = -dhdy*nf
				target[2] = nf
			end,
		},
		{ 'export',
			offset = -1,
			factor = 2,
			gamma = 1,
			file = mainconf.output or 'output.png',
		},
	},
}
