--[[stream start]]
--[[test.exl]]
--[[1:4]]
	local r1 = 394
	local d2i1 = r1
--[[2:4]]
	local r2 = function()
--[[stream start]]
	local d3i1
--[[3:7]]
	local r1 = 394
	local d4i1 = r1
--[[4:7]]
	local d4i2
--[[5:7]]
	local r2 = 395
	d4i2 = r2
--[[6:7]]
	local r3 = d4i2
	local r4 = 10
	local r5 = r3 + r4
	d3i1 = r5
	local r6 = d3i1
	do return r6 end
--[[stream end]]
		end --[[function]]
	local d2i2 = r2
--[[stream end]]