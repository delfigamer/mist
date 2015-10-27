local mainconf = require('main')
local radius = mainconf.radius or 1
local scalefactor = 2*radius + 1
local rfactor = mainconf.rfactor or 0.2
local rpower = mainconf.rpower or 1

return {
	{
		{ 'generate',
			width = 2*radius + 1,
			height = 2*radius + 1,
			planes = 1,
			genf = function(x, y)
				x, y = x - radius, y - radius
				local r = math.sqrt(x*x + y*y)
				local v = 1-rfactor*math.pow(r, rpower)
				if v < 0 then
					v = 0
				end
				return v
			end,
		},
		pin = 'kernel',
	},
	{
		{ 'import',
			file = mainconf.input or 'input.png',
		},
		pin = 'source',
	},
	{
		[0] = 'source',
		{ 'resample',
			widthfactor = scalefactor,
			heightfactor = scalefactor,
			weightf = function(t)
				if t < 0.5 then
					return 0
				else
					return 1
				end
			end,
		},
		{ 'functional',
			planes = 1,
			operand = 'kernel',
			func = function(target, x, y, ifmat, afmat)
				local dx = math.mod(x, scalefactor)
				local dy = math.mod(y, scalefactor)
				local factor = afmat.data[dx][dy][0]
				local source = ifmat.data[x][y]
				target[0] = factor*source[0]
				target[1] = factor*source[1]
				target[2] = factor*source[2]
			end,
		},
		{ 'export',
			file = mainconf.output or 'output.png',
		},
	},
}
