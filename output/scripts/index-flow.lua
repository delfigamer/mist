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
local size_height = 64

local wave_color = img.fmat:spawn(size_width, size_height, 3)
local wave = img.fmat:spawn(size_width, size_height, 2)
local wave_next = img.fmat:spawn(size_width, size_height, 2)
local potential = img.fmat:spawn(size_width, size_height, 1)

--[=[
Base equation:
	D[wave,t] = I hr/(2 m) laplacian[wave] - I V/hr wave
	|           ^ flow                       ^ rotation
	D[wave,t] = I ff laplacian[wave] - I rf wave
	laplacian[wave] = div[grad[wave]]

In terms of thrermal flow:
	[-d/2] ==k== [ d/2]
	flow = - k (b-a)
	D[a,t] =  k (b-a)
	D[b,t] = -k (b-a)

	a = - d/2 E^(-2 k t)
	b =   d/2 E^(-2 k t)
	(b-a) = d E^(-2 k t)
	D[a,t] =  d k E^(-2 k t)
	D[b,t] = -d k E^(-2 k t)
	Delta[a] = d/2 * (1 - E^(-2 k t))
With k == I ff:
	Delta[a] = d/2 * (1 - E^(-2 I ff t))
	sff = (1 - E^(-2 I ff t))
	sff = 1 - Cos[2 ff t] + I Sin[2 ff t]

	a = - d/2 (Cos[2 ff t] - I Sin[2 ff t])
	b =   d/2 (Cos[2 ff t] - I Sin[2 ff t])
]=]
local mass = 0.01
local planck = 1e0
local flow_factor = planck/(2*mass)
local rotation_factor = 1/planck
local time_step = 0.01
local time_stepcount = 9
local time_substeps = 1000
local deltatime = time_step/time_substeps
local flow_stepfactor_re = 0.5*(1-math.cos(2*flow_factor*deltatime))
local flow_stepfactor_im = 0.5*math.sin(2*flow_factor*deltatime)
local view_colorscale = 20

for x = 0, size_width-1 do
	for y = 0, size_height-1 do
		local a = math.sin(40*math.pi*x/(size_width))
		local b = math.sin(30*math.pi*y/(size_height))
		wave.data[y][x][0] = math.random() - math.random()
		wave.data[y][x][1] = 0
		potential.data[y][x][0] = 10*(x-size_width/2)^2 + 0*(y-size_height/2)^2
	end
end

-- normalize
do
	local total = 0
	for x = 0, size_width-1 do
		for y = 0, size_height-1 do
			total = total + wave.data[y][x][0]^2 + wave.data[y][x][1]^2
		end
	end
	for x = 0, size_width-1 do
		for y = 0, size_height-1 do
			wave.data[y][x][0] = wave.data[y][x][0] / math.sqrt(total)
			wave.data[y][x][1] = wave.data[y][x][1] / math.sqrt(total)
		end
	end
end

local function export_wave(path)
	local frer = 1
	local freg = -0.5
	local freb = -0.5
	local fimr = 0
	local fimg = 0.866
	local fimb = -0.866
	for x = 0, size_width-1 do
		for y = 0, size_height-1 do
			local re = wave.data[y][x][0] * view_colorscale
			local im = wave.data[y][x][1] * view_colorscale
			local abs = math.sqrt(re*re + im*im)
			-- if abs > 1 then
				-- re = re / abs
				-- im = im / abs
			-- end
			-- wave_color.data[y][x][0] = re*frer + im*fimr
			-- wave_color.data[y][x][1] = re*freg + im*fimg
			-- wave_color.data[y][x][2] = re*freb + im*fimb
			wave_color.data[y][x][0] = abs*abs
			wave_color.data[y][x][1] = abs*abs
			wave_color.data[y][x][2] = abs*abs
		end
	end
	img.export{
		path,
		wave_color}
end

export_wave('flow/00000.png')

for step = 1, time_stepcount do
	for substep = 1, time_substeps do
		for cx = 0, size_width-1 do
			for cy = 0, size_height-1 do
				local px = (cx-1) % size_width
				local nx = (cx+1) % size_width
				local py = (cy-1) % size_height
				local ny = (cy+1) % size_height
				local center_re = wave.data[cy][cx][0]
				local center_im = wave.data[cy][cx][1]
				local lap_re =
						( wave.data[py][cx][0]
						+ wave.data[ny][cx][0]
						+ wave.data[cy][px][0]
						+ wave.data[cy][nx][0])
					- center_re * 4
				local lap_im =
						( wave.data[py][cx][1]
						+ wave.data[ny][cx][1]
						+ wave.data[cy][px][1]
						+ wave.data[cy][nx][1])
					- center_im * 4
				local flowd_re = lap_re * flow_stepfactor_re
					- lap_im * flow_stepfactor_im
				local flowd_im = lap_re * flow_stepfactor_im
					+ lap_im * flow_stepfactor_re
				local next_re = center_re + flowd_re
				local next_im = center_im + flowd_im
				local darg = deltatime * rotation_factor
					* potential.data[cy][cx][0]
				local darg_re =   math.cos(darg)
				local darg_im = - math.sin(darg)
				-- local d_re = time_inc * (- flow_factor * lap_im
					-- + rotation_factor * potential.data[y][x][0] * val_im)
				-- local d_im = time_inc * (flow_factor * lap_re
					-- - rotation_factor * potential.data[y][x][0] * val_re)
				wave_next.data[cy][cx][0] = next_re * darg_re - next_im * darg_im
				wave_next.data[cy][cx][1] = next_re * darg_im + next_im * darg_re
			end
		end
		wave_next, wave = wave, wave_next
	end
	export_wave(string.format('flow/%.5i.png', step))
	local total = 0
	for x = 0, size_width-1 do
		for y = 0, size_height-1 do
			total = total + wave.data[y][x][0]^2 + wave.data[y][x][1]^2
		end
	end
	total = math.sqrt(total)
	for x = 0, size_width-1 do
		for y = 0, size_height-1 do
			wave.data[y][x][0] = wave.data[y][x][0] / total
			wave.data[y][x][1] = wave.data[y][x][1] / total
		end
	end
	print('step:', step, 'total:', total)
end
