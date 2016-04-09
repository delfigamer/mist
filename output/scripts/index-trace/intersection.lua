local modname = ...
local ffipure = require('base.ffipure')
local intersection = ffipure:module(modname)
local ffi = require('ffi')
local triangle = require(modname, 1, 'triangle')
local vector = require(modname, 1, 'vector')

ffi.cdef[[
typedef struct {
	triangle_t const* triangle;
	vector_t const* ray_origin;
	vector_t const* ray_direction;
	vector_t position;
	float weight_a;
	float weight_b;
	float weight_c;
	float ray_dist;
} intersection_t;
]]

intersection.typedef = 'intersection_t'

function intersection:init(triangle, ray_origin, ray_direction)
	self.triangle = triangle
	self.ray_origin = ray_origin
	self.ray_direction = ray_direction
	local ax, ay, az = triangle.a.x, triangle.a.y, triangle.a.z
	local bx, by, bz = triangle.b.x, triangle.b.y, triangle.b.z
	local cx, cy, cz = triangle.c.x, triangle.c.y, triangle.c.z
	local ox, oy, oz = ray_origin.x, ray_origin.y, ray_origin.z
	local dx, dy, dz = ray_direction.x, ray_direction.y, ray_direction.z
	local abxy = ax*by-ay*bx
	local abyz = ay*bz-az*by
	local abzx = az*bx-ax*bz
	local bcxy = bx*cy-by*cx
	local bcyz = by*cz-bz*cy
	local bczx = bz*cx-bx*cz
	local caxy = cx*ay-cy*ax
	local cayz = cy*az-cz*ay
	local cazx = cz*ax-cx*az
	local oaxy = ox*ay-oy*ax
	local oayz = oy*az-oz*ay
	local oazx = oz*ax-ox*az
	local obxy = ox*by-oy*bx
	local obyz = oy*bz-oz*by
	local obzx = oz*bx-ox*bz
	local ocxy = ox*cy-oy*cx
	local ocyz = oy*cz-oz*cy
	local oczx = oz*cx-ox*cz
	local divisor =
		dx*(abyz+bcyz+cayz) + dy*(abzx+bczx+cazx) + dz*(abxy+bcxy+caxy)
	if math.abs(divisor) <= 1e-6 then
		return
	end
	local wa =
		dx*(obyz+bcyz-ocyz) + dy*(obzx+bczx-oczx) + dz*(obxy+bcxy-ocxy)
	local wb =
		dx*(-oayz+ocyz+cayz) + dy*(-oazx+oczx+cazx) + dz*(-oaxy+ocxy+caxy)
	local wc =
		dx*(abyz-obyz+oayz) + dy*(abzx-obzx+oazx) + dz*(abxy-obxy+oaxy)
	local wd =
		ox*(abyz+bcyz+cayz) + oy*(abzx+bczx+cazx) + oz*(abxy+bcxy+caxy) -
		ax*bcyz - ay*bczx - az*bcxy
	self.weight_a = wa / divisor
	self.weight_b = wb / divisor
	self.weight_c = wc / divisor
	local D = - wd / divisor
	self.ray_dist = D
	self.position.x = ox + D * dx
	self.position.y = oy + D * dy
	self.position.z = oz + D * dz
end

intersection:buildmetatype()
