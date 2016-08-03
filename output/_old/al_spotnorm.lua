local targetsize = 256
local sourceheight = 1
local angle = 60
local minradius = math.tan(math.rad(angle))*sourceheight
local intensity = 1
local gamma = 1
local maxdist = math.sqrt(minradius*minradius + sourceheight*sourceheight)
local minv = 1/(maxdist*maxdist)

return {
	{
		{ 'generate',
			width = targetsize,
			height = targetsize,
			planes = 3,
			genf = function(x, y)
				local nx, ny, nz = (1 - 2*x/targetsize)*minradius, (1 - 2*y/targetsize)*minradius, sourceheight
				local r = math.sqrt(nx*nx + ny*ny + nz*nz)
				local v = (1/(r*r) - minv) / (1-minv)
				if v < 0 then
					v = 0
				end
				v = math.pow(v, gamma)
				nx, ny, nz = v*nx/r, v*ny/r, v*nz/r
				return nx, ny, nz
			end,
		},
		{ 'export',
			offset = -1,
			factor = 2,
			gamma = 1,
			file = 'resources\\common\\spot_normal_60.png',
		},
	},
}
