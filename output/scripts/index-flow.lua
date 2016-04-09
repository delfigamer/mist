local modname = ...
local index = package.modtable(modname)
local ffi = require('ffi')
local img = {
	export = require('img.export'),
	fmat = require('img.fmat'),
	load = require('img.load'),
	save = require('img.save'),
}

local size_width = 256
local size_height = 8

local concentration = img.fmat:spawn(size_width, size_height, 1)
local concentration_next = img.fmat:spawn(size_width, size_height, 1)
local temperature = img.fmat:spawn(size_width, size_height, 1)
local generation = img.fmat:spawn(size_width, size_height, 1)
local pressure = img.fmat:spawn(size_width, size_height, 1)
local flow = img.fmat:spawn(size_width, size_height, 2)
local invcapacity = img.fmat:spawn(size_width, size_height, 1)
local conductance = img.fmat:spawn(size_width, size_height, 3)

local air_capacity = 1e1
local air_conductance = 1e4
local liquid_capacity = 1e-1
local liquid_conductance = 1e2
local solid_capacity = 1e-2
local solid_conductance = 1e-2
local time_step = 1e-3
local time_maxsubstep = time_step / 16
local time_stepcount = 9999 -- math.huge
local time_maxreldn = 0.1
-- c = log_p(a x + b)
-- a x0 + b = 1
-- a x1 + b = p
-- a = (p - 1) / (x1 - x0)
-- b = 1 - a x0
-- x0 = 0.001
-- x1 = 1e2
-- local view_power = math.exp(1)
-- local view_factor = (view_power - 1) / (1e2 - 0.001)
-- local view_offset = 1 - view_factor * 0.001
local view_factor = 1
local view_offset = -0.001
local view_encode24 = true
local view_encode24log = false

local tube_width = 1
local generator_power = 0

for x = 0, size_width-1 do
	local cap = liquid_capacity +
		(air_capacity - liquid_capacity) * x / (size_width-1)
	local cond = liquid_conductance +
		(air_conductance - liquid_conductance) * x / (size_width-1)
	local val = 0.001 + x / (size_width-1)
	for y = 0, size_height-1 do
		concentration.data[y][x][0] = val
		generation.data[y][x][0] = 0
		temperature.data[y][x][0] = 1
		invcapacity.data[y][x][0] = 1 / cap
		conductance.data[y][x][0] = cond
	end
end

for y = 1, size_height-2 do
	generation.data[y][1][0] = generator_power
end

-- outer walls
for x = 0, size_width-1 do
	concentration.data[0][x][0] = 0
	concentration.data[size_height-1][x][0] = 0
	conductance.data[0][x][0] = 0
	conductance.data[size_height-1][x][0] = 0
end
for y = 1, size_height-2 do
	concentration.data[y][0][0] = 0
	concentration.data[y][size_width-1][0] = 0
	conductance.data[y][0][0] = 0
	conductance.data[y][size_width-1][0] = 0
end

for x = 0, size_width-1 do
	for y = 0, size_height-1 do
		local nx = math.mod(x+1, size_width)
		local ny = math.mod(y+1, size_height)
		conductance.data[y][x][1] = math.sqrt(
			conductance.data[y][x][0] * conductance.data[y][nx][0])
		conductance.data[y][x][2] = math.sqrt(
			conductance.data[y][x][0] * conductance.data[ny][x][0])
		-- concentration.data[y][x][0] = 0.0001 / invcapacity.data[y][x][0]
	end
end

img.export{
	'flow/0000.png',
	concentration,
	factor = view_factor,
	offset = view_offset,
	power = view_power,
	encode24 = view_encode24,
	encode24log = view_encode24log,
}

local substep_inc = time_maxsubstep
for time = 1, time_stepcount do
	local substep_count = 0
	local substep_passed = 0
	while substep_passed < time_step do
		time_inc = math.min(substep_inc, time_step - substep_passed)
		for x = 0, size_width-1 do
			for y = 0, size_height-1 do
				pressure.data[y][x][0] =
					concentration.data[y][x][0] *
					invcapacity.data[y][x][0] *
					temperature.data[y][x][0]
			end
		end
		for x = 0, size_width-1 do
			for y = 0, size_height-1 do
				local nx = math.mod(x+1, size_width)
				local ny = math.mod(y+1, size_height)
				local dpdx = pressure.data[y][nx][0] - pressure.data[y][x][0]
				local dpdy = pressure.data[ny][x][0] - pressure.data[y][x][0]
				flow.data[y][x][0] = - dpdx * conductance.data[y][x][1]
				flow.data[y][x][1] = - dpdy * conductance.data[y][x][2]
			end
		end
		local maxreldn = 0
		local next_valid = true
		for x = 0, size_width-1 do
			for y = 0, size_height-1 do
				local px = math.mod(x+size_width-1, size_width)
				local py = math.mod(y+size_height-1, size_height)
				local dn = (flow.data[y][px][0] + flow.data[py][x][1] -
					flow.data[y][x][0] - flow.data[y][x][1] +
					generation.data[y][x][0]) * time_inc
				local reldn = math.abs(dn) / concentration.data[y][x][0]
				if reldn > time_maxreldn then
					next_valid = false
				end
				maxreldn = math.max(maxreldn, reldn)
				if next_valid then
					concentration_next.data[y][x][0] =
						concentration.data[y][x][0] + dn
				end
			end
		end
		maxreldn = maxreldn * substep_inc / time_inc
		if next_valid then
			while maxreldn * 4 < time_maxreldn and substep_inc < time_maxsubstep do
				maxreldn = maxreldn * 2
				substep_inc = substep_inc * 2
			end
			concentration_next, concentration = concentration, concentration_next
			substep_passed = substep_passed + time_inc
			substep_count = substep_count + 1
		else
			while
				maxreldn > time_maxreldn
			do
				maxreldn = maxreldn * 0.5
				substep_inc = substep_inc * 0.5
			end
		end
	end
	img.export{
		string.format('flow/%.4i.png', time), concentration,
		factor = view_factor,
		offset = view_offset,
		power = view_power,
		encode24 = view_encode24,
		encode24log = view_encode24log,
	}
	print('step:', time, 'substeps:', substep_count)
end
