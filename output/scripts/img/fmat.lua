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
