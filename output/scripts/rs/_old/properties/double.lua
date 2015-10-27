local modname = ...
local double = package.modtable(modname)
local crc32 = require('crc32')
local ffi = require('ffi')
local int = require('rs.properties.int')

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

function double.write(f, stream)
	local u = fi64_t(f)
	int.write(u.h, stream)
	int.write(u.l, stream)
end

function double.read(stream)
	local u = fi64_t()
	u.h = int.write(stream)
	u.l = int.write(stream)
	return u.f
end

double.typeid = crc32('double')
