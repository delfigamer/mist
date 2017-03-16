local modname = ...
local fmat = require('img.fmat')

local function reduce(it)
	local ifmat = it[1]
	local width = math.floor(ifmat.header.width / 2)
	local height = math.floor(ifmat.header.height / 2)
	local ofmat = fmat:spawn(width, height, ifmat.header.planes)
	for y = 0, ofmat.header.height - 1 do
		for x = 0, ofmat.header.width - 1 do
			for p = 0, ofmat.header.planes - 1 do
				ofmat.data[y][x][p] = 0.25 * (
					ifmat.data[2*y  ][2*x  ][p] +
					ifmat.data[2*y+1][2*x  ][p] +
					ifmat.data[2*y  ][2*x+1][p] +
					ifmat.data[2*y+1][2*x+1][p])
			end
		end
	end
	return ofmat
end

package.modtable(modname, reduce)
