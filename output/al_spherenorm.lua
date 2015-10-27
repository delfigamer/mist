local targetsize = 512

return {
	{
		{ 'generate',
			width = targetsize,
			height = targetsize,
			planes = 3,
			genf = function(x, y)
				local nx, ny = (1 - 2*x/targetsize), (1 - 2*y/targetsize)
				local nz = 1 - nx*nx - ny*ny
				if nz < 0 then
					return 0, 0, 0
				end
				return -nx, -ny, nz
			end,
		},
		{ 'export',
			offset = -1,
			factor = 2,
			gamma = 1,
			file = 'input.png',
		},
	},
}
