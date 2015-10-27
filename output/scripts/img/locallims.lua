local modname = ...
local fmat = require('img.fmat')

local function gen_limvalue(min)
	local initlimit = min and math.huge or -math.huge
	local func
	if min then
		func = math.min
	else
		func = math.max
	end
	return function(sfmat, plane, xb, yb, xe, ye)
		local value = initlimit
		for x = xb, xe do
			for y = yb, ye do
				value = func(value, sfmat.data[y][x][plane])
			end
		end
		return value
	end
end

local maxvalue = gen_limvalue(false)
local minvalue = gen_limvalue(true)

local function locallims(it)
	local ifmat = it[1]
	local tfmat = fmat:create(ifmat.header)
	local ofmat = fmat:create(ifmat.header)
	local limvalue = it.min and minvalue or maxvalue
	local radius = it.radius or 1
	local width = ifmat.header.width
	local height = ifmat.header.height
	local planes = ifmat.header.planes
	if width > 2 * radius + 1 then
		for x = 0, radius - 1 do
			for y = 0, height - 1 do
				for p = 0, planes - 1 do
					tfmat.data[y][x][p] = limvalue(
						ifmat, p, 0, y, x + radius, y)
				end
			end
		end
		for x = radius, width - radius - 1 do
			for y = 0, height - 1 do
				for p = 0, planes - 1 do
					tfmat.data[y][x][p] = limvalue(
						ifmat, p, x - radius, y, x + radius, y)
				end
			end
		end
		for x = width - radius, width - 1 do
			for y = 0, height - 1 do
				for p = 0, planes - 1 do
					tfmat.data[y][x][p] = limvalue(
						ifmat, p, x - radius, y, width - 1, y)
				end
			end
		end
	elseif width > radius + 1 then
		for x = 0, width - radius - 2 do
			for y = 0, height - 1 do
				for p = 0, planes - 1 do
					tfmat.data[y][x][p] = limvalue(
						ifmat, p, 0, y, x + radius, y)
				end
			end
		end
		for y = 0, height - 1 do
			for p = 0, planes - 1 do
				local value = limvalue(
					ifmat, p, 0, y, width - 1, y)
				for x = width - radius - 1, radius do
					tfmat.data[y][x][p] = value
				end
			end
		end
		for x = radius + 1, width - 1 do
			for y = 0, height - 1 do
				for p = 0, planes - 1 do
					tfmat.data[y][x][p] = limvalue(
						ifmat, p, x - radius, y, width - 1, y)
				end
			end
		end
	else
		for y = 0, height - 1 do
			for p = 0, planes - 1 do
				local value = limvalue(
					ifmat, p, 0, y, width - 1, y)
				for x = 0, width - 1 do
					tfmat.data[y][x][p] = value
				end
			end
		end
	end
	if height > 2 * radius + 1 then
		for x = 0, width - 1 do
			for y = 0, radius - 1 do
				for p = 0, planes - 1 do
					ofmat.data[y][x][p] = limvalue(
						tfmat, p, x, 0, x, y + radius)
				end
			end
		end
		for x = 0, width - 1 do
			for y = radius, height - radius - 1 do
				for p = 0, planes - 1 do
					ofmat.data[y][x][p] = limvalue(
						tfmat, p, x, y - radius, x, y + radius)
				end
			end
		end
		for x = 0, width - 1 do
			for y = height - radius, height - 1 do
				for p = 0, planes - 1 do
					ofmat.data[y][x][p] = limvalue(
						tfmat, p, x, y - radius, x, height - 1)
				end
			end
		end
	elseif height > radius + 1 then
		for x = 0, width - 1 do
			for y = 0, height - radius - 2 do
				for p = 0, planes - 1 do
					ofmat.data[y][x][p] = limvalue(
						tfmat, p, x, 0, x, y + radius)
				end
			end
		end
		for x = 0, width - 1 do
			for p = 0, planes - 1 do
				local value = limvalue(
					tfmat, p, x, 0, x, height - 1)
				for y = height - radius - 1, radius do
					ofmat.data[y][x][p] = value
				end
			end
		end
		for x = 0, width - 1 do
			for y = radius + 1, height - 1 do
				for p = 0, planes - 1 do
					ofmat.data[y][x][p] = limvalue(
						tfmat, p, x, y - radius, x, height - 1)
				end
			end
		end
	else
		for x = 0, width - 1 do
			for p = 0, planes - 1 do
				local value = limvalue(
					tfmat, p, x, 0, x, height - 1)
				for y = 0, height - 1 do
					ofmat.data[y][x][p] = value
				end
			end
		end
	end
	return ofmat
end

package.modtable(modname, locallims)
