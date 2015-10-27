local modname = ...
local fmat = require('img.fmat')

local function generate(it)
	local width = it.width or 1
	local height = it.height or 1
	local planes = it.planes or 1
	local widthfactor = it.widthfactor or 1
	local heightfactor = it.heightfactor or 1
	local genf = assert(it[1])
	local ofmat = fmat:spawn(width, height, planes)
	for x = 0, width - 1 do
		for y = 0, height - 1 do
				ofmat.data[y][x] = ofmat.celltype(genf(x, y))
		end
	end
	return ofmat
end

package.modtable(modname, generate)
