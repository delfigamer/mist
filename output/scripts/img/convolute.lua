local modname = ...
local fmat = require('img.fmat')

local function convalue(ifmat, kfmat, x, y, p, xs, ys, xe, ye)
	local value = 0
	for dx = xs, xe - 1 do
		for dy = ys, ye - 1 do
			value = value
				+ ifmat.data[y + dy][x + dx][p]
					* kfmat.data[dy][dx][0]
		end
	end
	return value
end

local function convolute(it)
	local ifmat = it[1]
	local kfmat = it[2]
	local wrap = it.wrap
	local iwidth = ifmat.header.width
	local iheight = ifmat.header.height
	local iplanes = ifmat.header.planes
	local kwidth = kfmat.header.width
	local kheight = kfmat.header.height
	local ofmat = fmat:spawn(iwidth + kwidth - 1, iheight + kheight - 1, iplanes)
	for x = 0, kwidth - 2 do
		for y = 0, kheight - 2 do
			for p = 0, iplanes - 1 do
				ofmat.data[y][x][p] = convalue(
					ifmat, kfmat, x - kwidth + 1, y - kheight + 1, p,
					kwidth - 1 - x, kheight - 1 - y, kwidth, kheight)
			end
		end
	end
	for x = kwidth - 1, iwidth - 1 do
		for y = 0, kheight - 2 do
			for p = 0, iplanes - 1 do
				ofmat.data[y][x][p] = convalue(
					ifmat, kfmat, x - kwidth + 1, y - kheight + 1, p,
					0, kheight - 1 - y, kwidth, kheight)
			end
		end
	end
	for x = iwidth, iwidth + kwidth - 2 do
		for y = 0, kheight - 2 do
			for p = 0, iplanes - 1 do
				ofmat.data[y][x][p] = convalue(
					ifmat, kfmat, x - kwidth + 1, y - kheight + 1, p,
					0, kheight - 1 - y, iwidth + kwidth - x - 1, kheight)
			end
		end
	end
	for x = 0, kwidth - 2 do
		for y = kheight - 1, iheight - 1 do
			for p = 0, iplanes - 1 do
				ofmat.data[y][x][p] = convalue(
					ifmat, kfmat, x - kwidth + 1, y - kheight + 1, p,
					kwidth - 1 - x, 0, kwidth, kheight)
			end
		end
	end
	for x = kwidth - 1, iwidth - 1 do
		for y = kheight - 1, iheight - 1 do
			for p = 0, iplanes - 1 do
				ofmat.data[y][x][p] = convalue(
					ifmat, kfmat, x - kwidth + 1, y - kheight + 1, p,
					0, 0, kwidth, kheight)
			end
		end
	end
	for x = iwidth, iwidth + kwidth - 2 do
		for y = kheight - 1, iheight - 1 do
			for p = 0, iplanes - 1 do
				ofmat.data[y][x][p] = convalue(
					ifmat, kfmat, x - kwidth + 1, y - kheight + 1, p,
					0, 0, iwidth + kwidth - x - 1, kheight)
			end
		end
	end
	for x = 0, kwidth - 2 do
		for y = iheight, iheight + kheight - 2 do
			for p = 0, iplanes - 1 do
				ofmat.data[y][x][p] = convalue(
					ifmat, kfmat, x - kwidth + 1, y - kheight + 1, p,
					kwidth - 1 - x, 0, kwidth, iheight + kheight - y - 1)
			end
		end
	end
	for x = kwidth - 1, iwidth - 1 do
		for y = iheight, iheight + kheight - 2 do
			for p = 0, iplanes - 1 do
				ofmat.data[y][x][p] = convalue(
					ifmat, kfmat, x - kwidth + 1, y - kheight + 1, p,
					0, 0, kwidth, iheight + kheight - y - 1)
			end
		end
	end
	for x = iwidth, iwidth + kwidth - 2 do
		for y = iheight, iheight + kheight - 2 do
			for p = 0, iplanes - 1 do
				ofmat.data[y][x][p] = convalue(
					ifmat, kfmat, x - kwidth + 1, y - kheight + 1, p,
					0, 0, iwidth + kwidth - x - 1, iheight + kheight - y - 1)
			end
		end
	end
	if it[3] then
		return convolute{ofmat, unpack(it, 3)}
	else
		return ofmat
	end
end

package.modtable(modname, convolute)
