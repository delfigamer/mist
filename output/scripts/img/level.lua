local modname = ...
local fmat = require('img.fmat')

local function level(it)
	local ifmat = it[1]
	local ifactor = it.ifactor or 1
	local ioffset = it.ioffset or 0
	local igamma = it.igamma or 1
	local ofactor = it.ofactor or 1
	local ooffset = it.ooffset or 0
	local ogamma = it.ogamma or 1
	local gamma = ogamma / igamma
	local ofmat = fmat:create(ifmat.header)
	for x = 0, ofmat.header.width - 1 do
		for y = 0, ofmat.header.height - 1 do
			for p = 0, ofmat.header.planes - 1 do
				ofmat.data[y][x][p] =
					math.pow((ifmat.data[y][x][p] - ioffset) / ifactor, gamma)
					* ofactor + ooffset
			end
		end
	end
	return ofmat
end

package.modtable(modname, level)
