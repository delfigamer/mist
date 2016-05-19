local modname = ...

local function strhex(input)
	local result = ''
	for i = 1, #input / 2 do
		result = result .. string.char(tonumber(input:sub(2*i-1, 2*i), 16))
	end
	return result
end

package.modtable(modname, strhex)
