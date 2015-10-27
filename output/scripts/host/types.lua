local modname = ...
module(modname, package.seeall)
local ffi = require('ffi')

color = ffi.typeof('float[4]')
datetime = ffi.typeof('int[6]')
dword = ffi.typeof([[
	union {
		uint64_t d;
		int64_t ds;
		struct {
			uint32_t l, h;
		};
	}
]])
if dword(1).l == 0 then
	dword = ffi.typeof([[
		union {
			uint64_t d;
			int64_t ds;
			struct {
				uint32_t h, l;
			};
		}
	]])
end
pvertex = ffi.typeof('Vertex*')
vertex = ffi.typeof('Vertex')
