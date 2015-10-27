local ffi = require('ffi')
local targetsize = math.pow(2, 11)
local octavefactor = 0.9
local distrpower = 1
local dampfactor = 1

math.randomseed(1)

local noisevalue = ffi.new(
	string.format('float[%i][%i]', targetsize, targetsize))

local function noisevaluewrap(x, y)
	if x >= targetsize then
		x = x - targetsize
	elseif x < 0 then
		x = x + targetsize
	end
	if y >= targetsize then
		y = y - targetsize
	elseif y < 0 then
		y = y + targetsize
	end
	return noisevalue[x][y]
end

local function random()
	local val = math.random() * 2 - 1
	if val >= 0 then
		return math.pow(val, distrpower)
	else
		return -math.pow(-val, distrpower)
	end
end

local function diamondpass(step, factor)
	-- print('diamondpass', step, factor)
	for x = 0, targetsize-1, step*2 do
		for y = step, targetsize-1, step*2 do
			local sum =
				noisevaluewrap(x - step, y) +
				noisevaluewrap(x + step, y) +
				noisevaluewrap(x, y - step) +
				noisevaluewrap(x, y + step)
			local value = sum * dampfactor / 4 + random() * factor
			-- print(x, y, value)
			noisevalue[x][y] = value
		end
	end
	for x = step, targetsize-1, step*2 do
		for y = 0, targetsize-1, step*2 do
			local sum =
				noisevaluewrap(x - step, y) +
				noisevaluewrap(x + step, y) +
				noisevaluewrap(x, y - step) +
				noisevaluewrap(x, y + step)
			local value = sum * dampfactor / 4 + random() * factor
			-- print(x, y, value)
			noisevalue[x][y] = value
		end
	end
end

local function boxpass(step, factor)
	-- print('boxpass', step, factor)
	for x = step, targetsize-1, step*2 do
		for y = step, targetsize-1, step*2 do
			local sum =
				noisevaluewrap(x - step, y - step) +
				noisevaluewrap(x - step, y + step) +
				noisevaluewrap(x + step, y - step) +
				noisevaluewrap(x + step, y + step)
			local value = sum * dampfactor / 4 + random() * factor
			-- print(x, y, value)
			noisevalue[x][y] = value
		end
	end
end

noisevalue[0][0] = random()
-- print(0, 0, noisevalue[0][0])
do
	local step = targetsize/2
	local factor = octavefactor
	while step >= 1 do
		boxpass(step, factor)
		diamondpass(step, factor)
		step = step / 2
		factor = factor * octavefactor
	end
end

return {
	{
		{ 'generate',
			width = targetsize,
			height = targetsize,
			planes = 1,
			genf = function(x, y)
				return noisevalue[x][y]
			end,
		},
		{ 'export',
			offset = -2,
			factor = 4,
			gamma = 1,
			file = 'output.png',
		},
	},
}
