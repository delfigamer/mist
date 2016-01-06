local d3i1 = require('base.object'):derive({}, "test.Point")
local d3i2
local d3i3 = function(class, x, y)
	local result
	result = class:new()
	result.d2i1 = x
	result.d2i2 = y
	return result
end
d3i2 = d3i3(d3i1, 10, 20)
local d3i4 = function(a, b)
	return d3i3(d3i1, a.d2i1 * b, a.d2i2 * b)
end
d3i2 = d3i4(d3i2, 2)
