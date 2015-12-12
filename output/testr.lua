--[[stream start]]
--[[test.exl]]
--[[1:4]]
	local d2i1 = r0
--[[2:4]]
	local d2i2 = r0
--[[3:4]]
	local d2i3 = r0
--[[8:4]]
	local d2i4
	local r1 = function(d3i1, d3i2)
--[[stream start]]
	local d3i3
--[[9:4]]
	local r1 = d3i3
	do return r1 end
--[[stream end]]
		end --[[function]]
	d2i4 = r1
--[[11:4]]
	local d2i5
	local r2 = function(d3i1, d3i2)
--[[stream start]]
	local d3i3
--[[12:4]]
	local r1 = d3i3
	local r2 = d3i1
	do return r1, r2 end
--[[stream end]]
		end --[[function]]
	d2i5 = r2
--[[14:4]]
	local r3 = 0
	local d2i6 = r3
--[[15:4]]
	local r4 = d2i4
	local r5 = 1
	local r6 = 0
	local r7 = r4(r5, r6)
--[[16:4]]
	local r8 = d2i5
	local r9 = d2i6
	local r10 = 0
	local r11, r12 = r8(r9, r10)
	d2i6 = r12
--[[stream end]]