--[[stream start]]
--[[test.exl]]
--[[1:4]]
	local d2i1
	local r1 = function(d3i1, d3i2)
--[[stream start]]
	local d3i3
--[[2:7]]
	local r1 = d3i1
	local r2 = d3i2
	local r3 = r1 + r2
	d3i1 = r3
	d3i3 = r3
--[[3:4]]
	local r4 = d3i3
	local r5 = d3i1
	do return r4, r5 end
--[[stream end]]
		end --[[function]]
	d2i1 = r1
--[[5:4]]
	local r2 = 394
	local d2i2 = r2
--[[6:4]]
	local r3 = 0
	local d2i3 = r3
--[[7:4]]
	local r4 = d2i1
	local r5 = d2i3
	local r6 = 1
	local r7, r8 = r4(r5, r6)
	d2i3 = r8
--[[stream end]]