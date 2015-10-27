local modname = ...
local fmat = require('img.fmat')

local function frame(it)
	local ifmat = it[1]
	local iwidth = ifmat.header.width
	local iheight = ifmat.header.height
	local iplanes = ifmat.header.planes
	local dleft = it[2] or 0
	local dbottom = it[3] or 0
	local dright = it[4] or 0
	local dtop = it[5] or 0
	local width = iwidth - dleft + dright
	local height = iheight - dbottom + dtop
	local ofmat = fmat:spawn(width, height, iplanes)
	local fill = it.fill and ofmat.celltype(it.fill) or ofmat.celltype()
	local left = dleft <= 0 and -dleft or 0
	local bottom = dbottom <= 0 and -dbottom or 0
	local right = dright >= 0 and width - dright or width
	local top = dtop >= 0 and height - dtop or height
	for x = 0, left - 1 do
		for y = 0, height - 1 do
			ofmat.data[y][x] = fill
		end
	end
	for x = right, width - 1 do
		for y = 0, height - 1 do
			ofmat.data[y][x] = fill
		end
	end
	for x = left, right - 1 do
		for y = 0, bottom - 1 do
			ofmat.data[y][x] = fill
		end
	end
	for x = left, right - 1 do
		for y = top, height - 1 do
			ofmat.data[y][x] = fill
		end
	end
	for x = left, right - 1 do
		for y = bottom, top - 1 do
			ofmat.data[y][x] = ifmat.data[y + dbottom][x + dleft]
		end
	end
	return ofmat
end

package.modtable(modname, frame)
