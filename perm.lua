local ffi = require('ffi')

local function perm(max, n, seed)
	local source = ffi.new('int[?]', max)
	for i = 0, max-1 do
		source[i] = i+1
	end
	local result = ffi.new('int[?]', n)
	for i = 0, n-1 do
		local index = seed % max
		result[i] = source[index]
		source[index] = source[max-1]
		seed = math.floor(seed / max)
		max = max - 1
	end
	return result
end

