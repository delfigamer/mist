local modname = ...

local function enum(list)
	local t = {}
	for name, val in pairs(list) do
		t[name] = val
		t[val] = name
	end
	return t
end

package.modtable(modname, enum)
