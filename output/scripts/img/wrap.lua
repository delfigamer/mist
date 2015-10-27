local modname = ...
local fmat = require('img.fmat')

local function wrap(it)
	local ifmat = it[1]
	local iwidth = ifmat.header.width
	local iheight = ifmat.header.height
	local iplanes = ifmat.header.planes
	local width = it[2] or ifmat.width
	local height = it[3] or ifmat.height
	local ofmat = fmat:spawn(width, height, iplanes)
	for x = 0, width - 1 do
		for y = 0, height - 1 do
			for p = 0, iplanes - 1 do
				local value = 0
				for dx = x, iwidth - 1, width do
					for dy = y, iheight - 1, height do
						value = value + ifmat.data[dy][dx][p]
					end
				end
				ofmat.data[y][x][p] = value
			end
		end
	end
	return ofmat
end

package.modtable(modname, wrap)
