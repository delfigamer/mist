local modname = ...
local object = require('base.object')
local fmat = object:module(modname)
local ffi = require('ffi')
local fileio = require('rs.fileio')
local invoke = require('base.invoke')
local iostream = require('rs.iostream')

fmat.headertype = ffi.typeof[[
	struct{
		uint32_t width;
		uint32_t height;
		uint32_t planes;
		uint32_t reserved12;
	}
]]

function fmat:init(header)
	self.header = self.headertype(header)
	local datatype = ffi.typeof('float[$][$][$]', header.height, header.width, header.planes)
	self.data = datatype()
	self.celltype = ffi.typeof('float[$]', header.planes)
	self.datasize = header.width * header.height * header.planes * ffi.sizeof('float')
end

function fmat:spawn(width, height, planes)
	local header = self.headertype(width, height, planes, 0)
	return self:create(header)
end

local function read_main(self, path)
	local io = fileio:create(path, 'r')
	local stream = iostream:create(io)
	local header = self.headertype()
	if stream:read(16, header) ~= 16 then
		error('i/o error')
	end
	local fmat = self:create(header)
	if stream:read(fmat.datasize, fmat.data) ~= fmat.datasize then
		error('i/o error')
	end
	stream:release()
	io:release()
	return fmat
end

function fmat:read(path)
	return invoke(read_main, self, path)
end

local function write_main(self, path)
	local io = fileio:create(path, 'w')
	local stream = iostream:create(io)
	if stream:write(16, self.header) ~= 16 then
		error('i/o error')
	end
	if stream:write(self.datasize, self.data) ~= self.datasize then
		error('i/o error')
	end
	stream:release()
	io:release()
end

function fmat:write(path)
	invoke(write_main, self, path)
end

function fmat:get_xywrap(x, y, p)
	while x < 0 do
		x = x + self.header.width
	end
	while y < 0 do
		y = y + self.header.height
	end
	while x >= self.header.width do
		x = x - self.header.width
	end
	while y >= self.header.height do
		y = y - self.header.height
	end
	return self.data[y][x][p]
end

function fmat:get_xyclamp(x, y, p)
	if x < 0 then
		x = 0
	elseif x >= self.header.width then
		x = self.header.width - 1
	end
	if y < 0 then
		y = 0
	elseif y >= self.header.height then
		y = self.header.height - 1
	end
	return self.data[y][x][p]
end
