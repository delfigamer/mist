local modname = ...
module(modname, package.seeall)
local bit = require('bit')
local ffi = require('ffi')
local fileio = require('host.fileio')
local mainconf = require('main')
local stringinstance = require('host.stringinstance')
local pngreadtask = require('host.pngreadtask')
local pngwritetask = require('host.pngwritetask')
local window = require('host.window')

local basesize = mainconf.basesize or 16
local targetsize = mainconf.targetsize or 256
local octaveoffset = mainconf.octaveoffset or 0
local octaves = mainconf.octaves or math.floor(math.log(targetsize/basesize, 2) + octaveoffset + 1)
local coverage = mainconf.coverage or -5
local sharpness = mainconf.sharpness or 0.95
local linearfactor = mainconf.linearfactor or 1
local seed = mainconf.seed or os.time()
print('seed =', seed)
local outputfile = mainconf.output or 'output.png'

ffi.cdef[[
	typedef struct{ uint8_t r,g,b,a; } argb8;
	
	void memcpy( void* dest, void const* source, size_t length );
]]

local function bitmap(width, height, base)
	return ffi.new(string.format('%s[%i][%i]', base or 'argb8', width, height))
end

local function readpng(path)
	local io = fileio:create(_PATH .. path, 'rb')
	local task = pngreadtask:create(io)
	task:flush()
	local width, height, si = task:getwidth(), task:getheight(), task:getdata()
	local data = bitmap(width, height)
	ffi.C.memcpy(data, si:getdata(), width * height * 4)
	return width, height, data
end

local function writepng(pixels, width, height, path)
	local len = width * height * 4
	local si = stringinstance:create(len, len, ffi.cast('char const*', pixels))
	local io = fileio:create(_PATH .. path, 'wb')
	local task = pngwritetask:create(io, width, height, si)
	task:flush()
end

local function floatencode(f)
	local val = math.floor((1 - math.pow(sharpness, f - coverage)) * (linearfactor * 255) + 0.5)
-- 	local val = math.floor(f*32 + 0.5)
	if val < 0 then
		return 0
	elseif val > 255 then
		return 255
	else
		return val
	end
end

local function pixelencode(pixel, f)
	pixel.r = floatencode(f)
	pixel.g = pixel.r
	pixel.b = pixel.r
	pixel.a = 255
end

local function interpolate(a, b, x)
	x = (1 - math.cos(x * math.pi)) * 0.5
	return a*(1 - x) + b*x
end

local function baseindex(x)
	while x < 0 do
		x = x + basesize
	end
	while x >= basesize do
		x = x - basesize
	end
	return x
end

local function noisefunc(x, y, random)
	local n = bit.tobit(x + y * 57 + random * 121)
	n = bit.bxor(bit.lshift(n, 13), n)
	local m = bit.tobit(bit.tobit(n * n) * 15731 + 789221)
	n = bit.tobit(n * m + 1376312589)
	local r = 1.0 - bit.band(n, 0x7fffffff) * 0.000000000931322574615478515625
	return r
end

local function generatebase()
	local temp = bitmap(basesize, basesize, 'float')
	for x = 0, basesize-1 do
		for y = 0, basesize-1 do
			temp[x][y] = noisefunc(x, y, seed)
		end
	end
	local base = bitmap(basesize, basesize, 'float')
	for x = 0, basesize-1 do
		for y = 0, basesize-1 do
			local corners =
				temp[baseindex(x-1)][baseindex(y-1)] +
				temp[baseindex(x+1)][baseindex(y-1)] +
				temp[baseindex(x+1)][baseindex(y-1)] +
				temp[baseindex(x+1)][baseindex(y+1)]
			local sides =
				temp[baseindex(x-1)][y] +
				temp[x][baseindex(y-1)] +
				temp[baseindex(x+1)][y] +
				temp[x][baseindex(y+1)]
			local center =
				temp[x][y]
			base[x][y] = corners / 4 + sides / 2 + center
		end
	end
	return base
end

local function basevalue(base, nx, ny)
	local bx, by = basesize * nx, basesize * ny
	local wx, wy = baseindex(math.floor(bx)), baseindex(math.floor(by))
	local fx, fy = math.fmod(bx,1), math.fmod(by,1)
	local wx2, wy2 = baseindex(wx+1), baseindex(wy+1)
	local s1, s2, s3, s4 = base[wx][wy], base[wx2][wy], base[wx][wy2], base[wx2][wy2]
	local sd, su
	if fx <= 0.001 then
		sd, su = s1, s3
	elseif fx >= 0.999 then
		sd, su = s2, s4
	else
		sd, su = interpolate(s1, s2, fx), interpolate(s3, s4, fx)
	end
	if fy <= 0.001 then
		return sd
	elseif fy >= 0.999 then
		return su
	else
		return interpolate(sd, su, fy)
	end
end

local function generatetarget(base)
	local target = bitmap(targetsize, targetsize)
	local targetf = bitmap(targetsize, targetsize, 'float')
	print('target created, size =', targetsize)
	for x = 0, targetsize-1 do
		for y = 0, targetsize-1 do
			targetf[x][y] = 0
		end
	end
	local posfactor = math.pow(0.5, octaveoffset)/targetsize
	local valfactor = 1
	for octave = 1, octaves do
		for x = 0, targetsize-1 do
			for y = 0, targetsize-1 do
				targetf[x][y] = targetf[x][y] +
					basevalue(base, x*posfactor, y*posfactor) * valfactor
			end
		end
		print('octave added', octave)
		posfactor = posfactor * 2
		valfactor = valfactor / 2
	end
	for x = 0, targetsize-1 do
		for y = 0, targetsize-1 do
			pixelencode(target[x][y], targetf[x][y])
		end
	end
	return target
end

function register()
	do
		local base = generatebase()
		print('base generated, size =', basesize)
		local output = generatetarget(base)
		print('target generated')
		writepng(output, targetsize, targetsize, outputfile)
		print('finished')
		window:finish()
	end
	collectgarbage()
end

function unregister()
end
