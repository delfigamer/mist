local modname = ...
module(modname, package.seeall)
local extfunc = require('extfunc')
local ffi = require('ffi')
local fileio = require('host.fileio')
local mainconf = require('main')
local stringinstance = require('host.stringinstance')
local pngwritetask = require('host.pngwritetask')
local resourceset = require('rs.resourceset')
local window = require('host.window')

ffi.cdef[[
	typedef struct{ uint8_t r,g,b,a; } argb8;
	
	void memcpy( void* dest, void const* source, size_t length );
]]

local function bitmap(width, height, base)
	return ffi.new(string.format('%s[%i][%i]', base or 'argb8', height, width))
end

local function writepng(pixels, width, height, path)
	local len = width * height * 4
	local si = stringinstance:create(len, len, ffi.cast('char const*', pixels))
	local io = fileio:create(_PATH .. path, 'wb')
	local task = pngwritetask:create(io, width, height, si)
	task:flush()
end

function register()
	local font = resourceset:loadresource(mainconf.font or 'font.base16', 'Bitfont'):getfont()
	local line = mainconf.line or ''
	local rir = font.renderinfotype()
	rir.extraadvance = mainconf.extraadvance or 0
	rir.shearperiod = mainconf.shearperiod or 0
	font:getlinedims(line, rir)
	print(string.format([[
height = %i
descent = %i
width = %i
underhang = %i
overhang = %i]],
	rir.height, rir.descent, rir.width, rir.underhang, rir.overhang))
	local bm = bitmap(rir.width, rir.height)
	for x = 0, rir.width-1 do
		for y = 0, rir.height-1 do
			bm[y][x].r = 255
			bm[y][x].g = 255
			bm[y][x].b = 255
			bm[y][x].a = 255
		end
	end
	print('bitmap created')
	rir.bitmap = ffi.cast('uint32_t*', bm)
	rir.stride = rir.width
	rir.basex = rir.underhang
	rir.basey = rir.descent
	rir.value = 0xff000000
	rir.mask = mainconf.mask or 1
	print('RenderInfoRec created')
	font:render(line, rir)
	print('line rendered')
	writepng(bm, rir.width, rir.height, mainconf.output or 'output.png')
	print('file saved')
	window:finish()
end

function unregister()
end
