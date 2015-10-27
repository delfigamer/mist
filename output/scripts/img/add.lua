local modname = ...
local fmat = require('img.fmat')

local function add(it)
	local ifmat = it[1]
	local afmat = it[2]
	local ofmat = fmat:spawn(
		math.min(ifmat.header.width, afmat.header.width),
		math.min(ifmat.header.height, afmat.header.height),
		math.min(ifmat.header.planes, afmat.header.planes))
	for x = 0, ofmat.header.width - 1 do
		for y = 0, ofmat.header.height - 1 do
			for p = 0, ofmat.header.planes - 1 do
				ofmat.data[y][x][p] = ifmat.data[y][x][p] + afmat.data[y][x][p]
			end
		end
	end
	return ofmat
end

package.modtable(modname, add)
