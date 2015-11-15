local modname = ...

local counter = 0

return function()
	local cv = counter
	counter = counter + 1
	local id = ''
	repeat
		local ch = math.mod(cv, 26)
		cv = math.floor(cv / 26)
		id = string.char(ch + string.byte('a')) .. id
	until cv == 0
	return id
end
