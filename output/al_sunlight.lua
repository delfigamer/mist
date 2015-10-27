local mainconf = require('main')
local ambient = mainconf.ambient or {0, 0, 0}
local intensity = mainconf.intensity or
	{1 - ambient[1], 1 - ambient[2], 1 - ambient[3]}
local pitch = math.rad(mainconf.pitch or 90)
local yaw = math.rad(mainconf.yaw or 0)

local snx = math.cos(pitch)*math.cos(yaw)
local sny = math.cos(pitch)*math.sin(yaw)
local snz = math.sin(pitch)

local diffusegen
if mainconf.diffuse then
	diffusegen =
		{ 'import',
			file = mainconf.diffuse or 'diffuse.png',
		}
else
	diffusegen =
		{ 'generate',
			width = 1,
			height = 1,
			planes = 3,
			genf = function(x, y)
				return 1, 1, 1
			end
		}
end

return {
	{
		diffusegen,
		pin = 'diffuse',
	},
	{
		{ 'import',
			offset = -1,
			factor = 2,
			gamma = 1,
			file = mainconf.input or 'input.png',
		},
		{ 'functional',
			planes = 3,
			func = function(target, x, y, ifmat)
				local nx = ifmat.data[y][x][0]
				local ny = ifmat.data[y][x][1]
				local nz = ifmat.data[y][x][2]
				local i = snx*nx + sny*ny + snz*nz
				if i < 0 then
					i = 0
				end
				target[0] = ambient[1] + i * intensity[1]
				target[1] = ambient[2] + i * intensity[2]
				target[2] = ambient[3] + i * intensity[3]
			end,
		},
		{ 'modulate',
			operand = 'diffuse',
			defvalue = {[0] = 1, 1, 1},
		},
		{ 'export',
			file = mainconf.output or 'output.png',
		},
	},
}
