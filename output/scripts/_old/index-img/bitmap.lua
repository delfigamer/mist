local modname = ...
local object = require('base.object')
local bitmap = object:module(modname)
local databuffer = require('host.databuffer')
local ffi = require('ffi')
local fileio = require('rs.fileio')
local invoke = require('base.invoke')
local iostream = require('rs.iostream')
local host = require('host')
local pngptype = require('rs.pngptype')

ffi.cdef[[
	typedef struct{ uint8_t r,g,b,a; } rgba8;
]]

function bitmap:init(width, height)
	self.width = width
	self.height = height
	self.data = ffi.new(string.format('rgba8[%i][%i]', height, width))
end

local function readpng_main(self, path)
	local io = fileio:create(path, 'r')
	local stream = iostream:create(io)
	local bm = pngptype:read(stream)
	stream:release()
	io:release()
	local instance = self:create(bm.width, bm.height)
	ffi.copy(instance.data, bm.db:getdata(), bm.width * bm.height * 4)
	return instance
end

function bitmap:readpng(path)
	return invoke(readpng_main, self, path)
end

local function writepng_main(self, path)
	local len = self.width * self.height * 4
	local bm = {
		width = self.width,
		height = self.height,
		db = databuffer:create(len, len, ffi.cast('char const*', self.data)),
	}
	local io = fileio:create(path, 'w')
	local stream = iostream:create(io)
	pngptype:write(bm, stream)
	stream:release()
	io:release()
end

function bitmap:writepng(path)
	invoke(writepng_main, self, path)
end
