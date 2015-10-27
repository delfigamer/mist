local ffi = require('ffi')
local fileio = require('host.fileio')
local iostream = require('host.iostream')
local mainconf = require('main')
local pngreadtask = require('host.pngreadtask')

ffi.cdef[[
	typedef struct{ uint8_t r,g,b,a; } rgba8;

	void memcpy( void* dest, void const* source, size_t length );
]]

function register()
	local io = fileio:create(_PATH .. mainconf.path, 'rb')
	local stream = iostream:create(io, 0, 'read')
	local task = pngreadtask:create(stream)
	task:flush()
	local width, height, si = task:getwidth(), task:getheight(), task:getdata()
	local bmdata = ffi.cast(si:getdata(), 'rgba8*')
	local fdata = ffi.new('uint8_t[?]', width * height)
	for x = 0, width-1 do
		for y = 0, height-1 do
		end
	end
end
