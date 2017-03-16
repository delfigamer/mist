local modname = ...
local object = require('base.object')
local fmat = object:module(modname)
local databuffer = require('host.databuffer')
local ffi = require('ffi')

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
	local datatype = ffi.typeof('$*',
		ffi.typeof('float[$][$]', header.width, header.planes))
	self.datasize = header.width * header.height * header.planes *
		ffi.sizeof('float')
	self.buffer = databuffer:create(self.datasize, self.datasize)
	self.data = datatype(self.buffer:getdata())
	self.celltype = ffi.typeof('float[$]', header.planes)
end

function fmat:spawn(width, height, planes)
	local header = self.headertype(width, height, planes, 0)
	return self:create(header)
end
