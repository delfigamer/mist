--[[stream start]]
--[[test.exl]]
--[[1:4]]
	local r1 = 394
	local d2i1 = r1
--[[2:4]]
	local d2i2
	local r2 = function(d3i1, d3i2)
--[[stream start]]
	local d3i3
--[[3:7]]
	local r1 = d3i1
	local r2 = d3i2
	local r3 = r1 + r2
	d3i1 = r3
	d3i3 = r3
--[[4:7]]
	local r4 = d3i3
	local r5 = d3i1
	do return r4, r5 end
--[[stream end]]
		end --[[function]]
	d2i2 = r2
--[[5:4]]
	local d2i3
	local r3 = function()
--[[stream start]]
	local d3i1
--[[6:7]]
	local r1 = 10
	d3i1 = r1
--[[7:7]]
	local r2 = d3i1
	do return r2 end
--[[stream end]]
		end --[[function]]
	d2i3 = r3
--[[8:4]]
	local r4 = 394
	local d2i4 = r4
--[[9:4]]
	local r5 = d2i2
	local r6 = d2i4
	local r7 = 1
	local r8, r9 = r5(r6, r7)
	d2i4 = r9
	local d2i5 = r8
--[[10:4]]
	local r10 = d2i4
	local r11 = d2i3
	local r12 = r11()
	local r13 = r10 + r12
	d2i4 = r13
--[[stream end]]