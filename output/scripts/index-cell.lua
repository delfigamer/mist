local modname = ...
local index = package.modtable(modname)
local ffi = require('ffi')
local img = {
	export = require('img.export'),
	fmat = require('img.fmat'),
	load = require('img.load'),
	save = require('img.save'),
}

local size_width = 120
local size_height = 60
local display_scale_x = 8 * math.sqrt(3) / 2
local display_scale_y = 8
local display_width = math.floor(size_width * display_scale_x)
local display_height = math.floor(size_height * display_scale_y)

local fieldtype = ffi.typeof('int[$][$]', size_height, size_width)
local field = fieldtype()
local field_next = fieldtype()
local field_display = img.fmat:spawn(display_width, display_height, 3)
local colortable = {
	[0] = {0,   0,   0},
	[1] = {0.7, 0,   0},
	[2] = {0.7, 0.3, 0.3},
	[3] = {0.7, 0.7, 0.7},
	[4] = {0.3, 0.3, 0.7},
	[5] = {0,   0,   0.7},
}

for fy = 0, size_height-1 do
	for fx = 0, size_width-1 do
		field[fy][fx] = 0
	end
end
field[8][16] = 1
field[8][17] = 2
field[9][18] = 3
field[10][17] = 4
field[10][16] = 5

local function export_field(path)
	for dy = 0, display_height-1 do
		local cy = dy / display_scale_y
		local ly = math.fmod(cy, 2)
		local fy = cy - ly
		for dx = 0, display_width-1 do
			local cx = dx / display_scale_x
			local lx = math.fmod(cx, 1)
			local fx = cx - lx
			local fy = fy
			if ly < math.sqrt(1/3) then
			elseif ly < 1 then
				local ldx = (lx - 0.5) * 2
				local ldy = (ly - math.sqrt(1/3)) / (1 - math.sqrt(1/3))
				fx = fx + math.floor(math.abs(ldy + ldx))
				fy = fy + math.floor(ldy + math.abs(ldx))
			elseif ly < (1 + math.sqrt(1/3)) then
				fx = fx + math.floor(lx + 0.5)
				fy = fy + 1
			else
				local ldx = (lx - 0.5) * 2
				local ldy = (ly - (1 + math.sqrt(1/3))) / (1 - math.sqrt(1/3))
				fx = fx + math.floor(math.abs(ldx - ldy + 1))
				fy = fy + 2 + math.floor(ldy - math.abs(ldx))
			end
			fx = fx % size_width
			fy = fy % size_height
			local value = field[fy][fx]
			local color = colortable[value]
			field_display.data[dy][dx] = color
		end
	end
	img.export{
		path,
		field_display}
end

local function dostep()
	for fy = 0, size_height-1 do
		local dxl
		local dxr
		if fy % 2 == 0 then
			dxl = 0
			dxr = 1
		else
			dxl = size_width - 1
			dxr = 0
		end
		for fx = 0, size_width-1 do
			local xnl = (fx + (size_width-1)) % size_width
			local xnr = (fx + 1) % size_width
			local xl = (fx + dxl) % size_width
			local xr = (fx + dxr) % size_width
			local yd = (fy + (size_height - 1)) % size_height
			local yu = (fy + 1) % size_height
			local value = field[fy][fx]
				+ field[fy][xnl]
				+ field[fy][xnr]
				+ field[yd][xl]
				+ field[yd][xr]
				+ field[yu][xl]
				+ field[yu][xr]
			field_next[fy][fx] = value % 6
		end
	end
	field, field_next = field_next, field
end

export_field('flow/00000.png')
for step = 1, 100 do
	dostep()
	export_field(string.format('flow/%.5i.png', step))
	print('step:', step)
end
