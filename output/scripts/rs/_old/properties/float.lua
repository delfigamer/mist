local modname = ...
local float = package.modtable(modname)
local crc32 = require('crc32')
local ffi = require('ffi')
local int = require('rs.properties.int')

local fi32_t = ffi.typeof('union {\
	float f;\
	uint32_t i;\
}')

function float.write(f, stream)
	local u = fi32_t(f)
	int.write(u.i, stream)
end

function float.read(stream)
	local u = fi32_t()
	u.i = int.read(stream)
	return u.f
end

float.typeid = crc32('float')
