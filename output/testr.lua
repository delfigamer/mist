--[[stream start]]
--[[test.exl]]
--[[1:4]]
	local d2i1
	local r1 = 394
	d2i1 = r1
--[[2:4]]
	local d2i2
	local r2 = function(d3i1, d3i2)
--[[stream start]]
	local d3i3
--[[3:7]]
	local r1 = d3i1
	local r2 = d3i2
	local r3 = r1 + r2
	d3i3 = r3
--[[4:7]]
	local r4 = d3i3
	do return r4 end
--[[stream end]]
		end --[[function]]
	d2i2 = r2
--[[5:4]]
	local d2i3
	local r3 = function(d3i1, d3i2)
--[[stream start]]
	local d3i3
--[[6:7]]
	local r1 = d3i1
	local r2 = d3i2
	local r3 = r1 + r2
	d3i1 = r3
	d3i3 = r3
--[[7:7]]
	local r4 = d3i3
	local r5 = d3i1
	do return r4, r5 end
--[[stream end]]
		end --[[function]]
	d2i3 = r3
--[[8:4]]
	local d2i4
	local r4 = d2i2
	local r5 = 394
	local r6 = 100
	local r7 = r4(r5, r6)
	d2i4 = r7
--[[9:4]]
	local r8 = d2i3
	local r9 = d2i4
	local r10 = 1
	local r11, r12 = r8(r9, r10)
	d2i4 = r12
--[[stream end]]