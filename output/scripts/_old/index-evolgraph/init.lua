local modname = ...
module(modname, package.seeall)
local ffi = require('ffi')
local fileio = require('host.fileio')
local iostream = require('host.iostream')
local mainconf = require('main')
local pngwritetask = require('host.pngwritetask')
local stringinstance = require('host.stringinstance')
local window = require('host.window')

ffi.cdef[[
	typedef struct{ uint8_t r,g,b,a; } rgba8;
	typedef struct{ } FILE;

	void memcpy( void* dest, void const* source, size_t length );
	FILE* fopen( char const* path, char const* mode );
	void fclose( FILE* f );
	int fscanf( FILE* f, char const* format, ... );
]]

local function readdata(input, width)
	local lines = {}
	local height = 0
	while true do
		local line = ffi.new('int[?]', width)
		for i = 0, width-1 do
			if ffi.C.fscanf(input, '%i', line+i) ~= 1 then
				return lines, height
			end
		end
		lines[height] = line
		height = height + 1
	end
end

local function encode(value, pixel)
	local normvalue = math.tanh(value * (0.54930614433405 / 50))
	local nr, ng, nb
	if normvalue > 0 then
		nr = 1
		ng = 1 - normvalue
		nb = 1 - normvalue
	else
		nr = 1 + normvalue
		ng = 1 + normvalue
		nb = 1
	end
	pixel.r = math.floor(nr * 255 + 0.5)
	pixel.g = math.floor(ng * 255 + 0.5)
	pixel.b = math.floor(nb * 255 + 0.5)
	pixel.a = 255
end

local function render(lines, width, height, rawdata)
	local data = ffi.cast('rgba8*', rawdata)
	for y = 0, height - 1 do
		for x = 0, width - 1 do
			encode(lines[y][x], data[y*width+x])
		end
	end
end

local function action()
	local input = ffi.C.fopen(mainconf.input, 'r')
	assert(input ~= nil)
	local buf = ffi.new('int[1]')
	assert(ffi.C.fscanf(input, '%i', buf) == 1)
	local width = buf[0]
	local lines, height = readdata(input, width)
	local len = width * height * 4
	local si = stringinstance:create(len, len, nil)
	render(lines, width, height, si:getdata())
	local io = fileio:create(mainconf.output, 'wb')
	local stream = iostream:create(io, 0, 'write')
	local task = pngwritetask:create(stream, width, height, si)
	task:flush()
end

function register()
	local suc, err = pcall(action)
	if not suc then
		print(err)
	end
	window:finish()
end

function unregister()
end
