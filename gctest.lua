local ffi = require('ffi')

ffi.cdef[[
	typedef struct
	{
		int x, y;
	} point_t;
]]

local point = {}
point.instmeta = {__index = point}

function point:new(...)
	return self.metatype(...)
end

function point.instmeta:__tostring()
	return string.format('point< %i, %i >', self.x, self.y)
end

function point:release()
	print('release', self)
end

point.metatype = ffi.metatype('point_t', point.instmeta)

do
	local p1 = point:new(1, 2)
	local p2 = point:new(p1)
	local p3 = ffi.gc(point:new(3, 4), point.release)
	local p4 = point:new(p3)
	print(p1, p2, p3, p4)
end
collectgarbage()
