local modname = ...
local scalars = package.modtable(modname)
local dword = require('host.types').dword
local ffi = require('ffi')

-- types:
-- 0 - reserved
-- 1 - int32
-- 2 - uint32
-- 3 - int64
-- 4 - uint64
-- 5 - long/int
-- 6 - ulong/uint
-- 7 - float
-- 8 - double
-- 9 - string
-- non-serializable additional types:
-- int16
-- uint16
-- int8
-- uint8
-- when using these, you have to align larger types by 4 bytes yourself

local i32_t = ffi.typeof('uint8_t[4]')
local int8_t = ffi.typeof('int8_t')
local int16_t = ffi.typeof('int16_t')
local uint32_t = ffi.typeof('uint32_t')
local fi32_t = ffi.typeof('union {\
	float f;\
	uint32_t i;\
}')
local fi64_t
do
	local utest = ffi.typeof('union {\
		double f;\
		struct {\
			uint32_t l;\
			uint32_t h;\
		};\
	}')
	local ui = utest(1)
	if ui.l == 0 then
		fi64_t = utest
	else
		fi64_t = ffi.typeof('union {\
			double f;\
			struct {\
				uint32_t h;\
				uint32_t l;\
			}\
		};')
	end
end
local cbuf_t = ffi.typeof('char[?]')

local function itobe8(i)
	return i32_t(bit.tobit(i))
end

local function be16toi(i32)
	return i32[1]
end

local function itobe16(i)
	return i32_t(
		bit.rshift(i, 8),
		bit.tobit(i))
end

local function be16toi(i32)
	return bit.bor(
		bit.lshift(i32[0], 8),
		i32[1])
end

local function itobe32(i)
	return i32_t(
		bit.rshift(i, 24),
		bit.rshift(i, 16),
		bit.rshift(i, 8),
		bit.tobit(i))
end

local function be32toi(i32)
	return bit.bor(
		bit.lshift(i32[0], 24),
		bit.lshift(i32[1], 16),
		bit.lshift(i32[2], 8),
		i32[3])
end

local uint8 = {
}
scalars.uint8 = uint8

function uint8:write(i, stream)
	if stream:write(1, itobe8(i or 0)) ~= 1 then
		error('i/o failure')
	end
end

function uint8:read(stream)
	local buf = i32_t()
	if stream:read(1, buf) ~= 1 then
		error('i/o failure')
	end
	return be8toi(buf)
end

function uint8:skip(stream)
	if stream:skip(1) ~= 1 then
		error('i/o failure')
	end
end

local int8 = {
}
scalars.int8 = int8

int8.write = uint8.write

function int8:read(stream)
	return tonumber(int8_t(uint8:read(stream)))
end

int8.skip = uint8.skip

local uint16 = {
}
scalars.uint16 = uint16

function uint16:write(i, stream)
	if stream:write(2, itobe16(i or 0)) ~= 2 then
		error('i/o failure')
	end
end

function uint16:read(stream)
	local buf = i32_t()
	if stream:read(2, buf) ~= 2 then
		error('i/o failure')
	end
	return be16toi(buf)
end

function uint16:skip(stream)
	if stream:skip(2) ~= 2 then
		error('i/o failure')
	end
end

local int16 = {
}
scalars.int16 = int16

int16.write = uint16.write

function int16:read(stream)
	return tonumber(int16_t(uint16:read(stream)))
end

int16.skip = uint16.skip

local int32 = {
	typeid = 1,
	typename = 'int32',
}
scalars.int32 = int32
scalars[int32.typeid] = int32

function int32:write(i, stream)
	if stream:write(4, itobe32(i or 0)) ~= 4 then
		error('i/o failure')
	end
end

function int32:read(stream)
	local buf = i32_t()
	if stream:read(4, buf) ~= 4 then
		error('i/o failure')
	end
	return be32toi(buf)
end

function int32:skip(stream)
	if stream:skip(4) ~= 4 then
		error('i/o failure')
	end
end

local uint32 = {
	typeid = 2,
	typename = 'uint32',
}
scalars.uint32 = uint32
scalars[uint32.typeid] = uint32

uint32.write = int32.write

function uint32:read(stream)
	return tonumber(uint32_t(int32:read(stream)))
end

uint32.skip = int32.skip

local int64 = {
	typeid = 3,
	typename = 'int64',
}
scalars.int64 = int64
scalars[int64.typeid] = int64

function int64:write(i, stream)
	local u = dword(i)
	int32:write(u.h, stream)
	int32:write(u.l, stream)
end

function int64:read(stream)
	local u = dword()
	u.h = int32:read(stream)
	u.l = int32:read(stream)
	return u.ds
end

function int64:skip(stream)
	if stream:skip(8) ~= 8 then
		error('i/o failure')
	end
end

local uint64 = {
	typeid = 4,
	typename = 'uint64',
}
scalars.uint64 = uint64
scalars[uint64.typeid] = uint64

uint64.write = int64.write

function uint64:read(stream)
	local u = dword()
	u.h = int32:read(stream)
	u.l = int32:read(stream)
	return u.d
end

uint64.skip = int64.skip

local long = {
	typeid = 5,
	typename = 'long',
}
scalars.long = long
scalars[long.typeid] = long

function long:write(i, stream)
	local u = dword(i)
	if u.h == 0 and u.l <= 0x3fffffff then
		int32:write(u.l, stream)
	elseif u.h == 0xffffffff and u.l >= 0xc0000000 then
		int32:write(bit.band(u.l, 0x7fffffff), stream)
	elseif u.h <= 0x3fffffff then
		int32:write(bit.bor(u.h, 0x80000000), stream)
		int32:write(u.l, stream)
	elseif u.h >= 0xc0000000 then
		int32:write(u.h, stream)
		int32:write(u.l, stream)
	else
		error('invalid number')
	end
end

function long:read(stream)
	local lead = int32:read(stream)
	if bit.band(lead, 0xc0000000) == 0 then
		return lead
	elseif bit.band(lead, 0xc0000000) == 0x40000000 then
		return bit.bor(lead, 0x80000000)
	else
		local u = dword()
		if bit.band(lead, 0xc0000000) == 0x80000000 then
			u.h = bit.band(lead, 0x7fffffff)
		elseif bit.band(lead, 0xc0000000) == 0xc0000000 then
			u.h = lead
		end
		u.l = int32:read(stream)
		return u.ds
	end
end

function long:skip(stream)
	local lead = int32:read(stream)
	if bit.band(lead, 0x80000000) == 0x80000000 then
		int32:skip(stream)
	end
end

local ulong = {
	typeid = 6,
	typename = 'ulong',
}
scalars.ulong = ulong
scalars[ulong.typeid] = ulong

function ulong:write(i, stream)
	local u = dword(i)
	if u.h == 0 and u.l <= 0x7fffffff then
		int32:write(u.l, stream)
	elseif u.h <= 0x7fffffff then
		int32:write(bit.bor(u.h, 0x80000000), stream)
		int32:write(u.l, stream)
	else
		error('invalid number')
	end
end

function ulong:read(stream)
	local lead = int32:read(stream)
	if bit.bor(lead, 0x80000000) == 0 then
		return lead
	else
		local u = dword()
		u.h = bit.band(lead, 0x7fffffff)
		u.l = int32:read(stream)
		return u.d
	end
end

ulong.skip = long.skip

local int = {
	typeid = 5,
	typename = 'int',
}
scalars.int = int

int.write = long.write

function int:read(stream)
	return tonumber(long:read(stream))
end

int.skip = long.skip

local uint = {
	typeid = 6,
	typename = 'uint',
}
scalars.uint = uint

uint.write = ulong.write

function uint:read(stream)
	return tonumber(ulong:read(stream))
end

uint.skip = ulong.skip

local float = {
	typeid = 7,
	typename = 'int32',
}
scalars.float = float
scalars[float.typeid] = float

function float:write(f, stream)
	local u = fi32_t(f)
	int32:write(u.i, stream)
end

function float:read(stream)
	local u = fi32_t()
	u.i = int32:read(stream)
	return u.f
end

float.skip = int32.skip

local double = {
	typeid = 8,
	typename = 'double',
}
scalars.double = double
scalars[double.typeid] = double

function double:write(f, stream)
	local u = fi64_t(f)
	int32:write(u.h, stream)
	int32:write(u.l, stream)
end

function double:read(stream)
	local u = fi64_t()
	u.h = int32:read(stream)
	u.l = int32:read(stream)
	return u.f
end

double.skip = int64.skip

local str = {
	typeid = 9,
	typename = 'string',
}
scalars.string = str
scalars[str.typeid] = str

function str:write(str, stream)
	if not str then
		uint32:write(0, stream)
		return
	end
	local length = #str
	uint32:write(length, stream)
	if stream:write(length, str) ~= length then
		error('i/o failure')
	end
	local r = 4 - bit.band(length, 3)
	if r ~= 4 then
		if stream:skip(r) ~= r then
			error('i/o failure')
		end
	end
end

function str:read(stream)
	local length = uint32:read(stream)
	if length == 0 then
		return ''
	end
	local cbuf = cbuf_t(length)
	if stream:read(length, cbuf) ~= length then
		error('i/o failure')
	end
	local r = 4 - bit.band(length, 3)
	if r ~= 4 then
		if stream:skip(r) ~= r then
			error('i/o failure')
		end
	end
	return ffi.string(cbuf, length)
end

function str:skip(stream)
	local length = uint32:read(stream)
	if length == 0 then
		return
	end
	length = tonumber(uint32_t(bit.band(length + 3, 0xfffffffc)))
	if stream:skip(length) ~= length then
		error('i/o failure')
	end
end
