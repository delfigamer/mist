local engine_left = 1
local engine_right = 0
local sensor_left = 0
local sensor_right = 1

local light_factor = 50 / (255 - 70)
local light_offset = -70 * light_factor

local forward_base = 70
local forward_err_f = 0.05
local forward_dist_horz = 60

local light_line_threshold_up = 60
local light_line_threshold_down = 30

local light_left
local light_right
local light_min
local light_max

function light_update()
	light_left = Sensor(sensor_left) * light_factor + light_offset
	light_right = Sensor(sensor_right) * light_factor + light_offset
	if light_left < light_right then
		light_min = light_left
		light_max = light_right
	else
		light_min = light_right
		light_max = light_left
	end
	print(light_left, light_right)
end

function forward_line()
	local err = light_left - light_right
	err = err * forward_err_f
	OnFwd(engine_left, forward_base + err)
	OnFwd(engine_right, forward_base - err)
end

function rollout()
	repeat
		light_update()
		forward_line()
	until light_max < light_line_threshold_down
	repeat
		light_update()
		forward_line()
	until light_min > light_line_threshold_up
	StartTask(forward)
end

function forward()
	local counter_left = GetMotorCounter(engine_left) + forward_dist_horz
	local counter_right = GetMotorCounter(engine_right) + forward_dist_horz
	while GetMotorCounter(engine_left) < counter_left and
			GetMotorCounter(engine_right) < counter_right do
		light_update()
		forward_line()
	end
	OnFwd(engine_left, 0)
	OnFwd(engine_right, 0)
end

function main()
	Wait(3)
	StartTask(rollout)
end
