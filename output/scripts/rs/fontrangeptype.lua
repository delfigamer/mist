local modname = ...
local fontrangeptype = package.modtable(modname)
local crc32 = require('crc32')
local databuffer = require('host.databuffer')
local ffi = require('ffi')
local scalars = require('rs.scalars')

local chars_t = ffi.typeof[[
	struct {
		int width;
		int height;
		int xoffset;
		int yoffset;
		int advance;
		uint8_t* data;
	}[?] ]]
fontrangeptype.typename = modname
fontrangeptype.typeid = crc32(modname)

function fontrangeptype:write(data, stream)
	scalars.uint16:write(data.height, stream)
	scalars.uint16:write(data.descent, stream)
	scalars.uint16:write(data.maxcharwidth, stream)
	local datasize = data.db:getlength()
	scalars.uint16:write(bit.rshift(datasize + 255, 8), stream)
	scalars.uint32:write(data.firstchar, stream)
	scalars.uint32:write(data.firstchar + data.charcount, stream)
	for i = 0, data.charcount-1 do
		local char = data.chars[i]
		scalars.uint16:write(char.width, stream)
		scalars.uint16:write(char.height, stream)
		scalars.int16:write(char.xoffset, stream)
		scalars.int16:write(char.yoffset, stream)
		scalars.int16:write(char.advance, stream)
		scalars.uint16:write(0, stream)
		scalars.uint32:write(0, stream)
	end
	if stream:write(datasize, data.db:getdata()) ~= datasize then
		error('i/o error')
	end
end

function fontrangeptype:read(stream)
	local data = {}
	data.height = scalars.uint16:read(stream)
	data.descent = scalars.uint16:read(stream)
	data.maxcharwidth = scalars.uint16:read(stream)
	local datasize = scalars.uint16:read(stream) * 256
	data.firstchar = scalars.uint32:read(stream)
	data.charcount = scalars.uint32:read(stream) - data.firstchar
	data.chars = chars_t(data.charcount)
	data.db = databuffer:create(datasize, datasize)
	local ptr = ffi.cast('uint8_t*', data.db:getdata())
	local ptrbase = ptr
	for i = 0, data.charcount-1 do
		local char = data.chars[i]
		char.width = scalars.uint16:read(stream)
		char.height = scalars.uint16:read(stream)
		char.xoffset = scalars.int16:read(stream)
		char.yoffset = scalars.int16:read(stream)
		char.advance = scalars.int16:read(stream)
		scalars.uint16:skip(stream)
		scalars.uint32:skip(stream)
		local stride = bit.rshift(char.width + 7, 3)
		local datasize = bit.band(stride * char.height + 15, 0xfffffff0)
		char.data = ptr
		ptr = ptr + datasize
	end
	local actualsize = ptr - ptrbase
	if stream:read(actualsize, ptrbase) ~= actualsize then
		error('i/o error')
	end
	return data
end
