--[[stream start]]
--[[test.exl]]
--[[1:4]]
	local r1 = "foobar"
	local d2i1 = r1
--[[2:4]]
	local r2 = 42
	local d2i2 = r2
--[[3:4]]
	local d2i3 = r0
--[[4:4]]
	local d2i4
--[[5:4]]
	local r3 = 44
	d2i4 = r3
-- Rn
--[[6:4]]
	local d2i5 = r0
--[[10:4]]
	local d2i6 = r0
--[[11:4]]
	local d2i7
--[[12:4]]
	local r4 = function(d3i1, d3i2)
--[[stream start]]
	local d3i3
--[[14:7]]
	local r1 = d3i1
	local r2 = d3i2
	local r3 = r1 + r2
-- Rn
	d3i3 = r3
-- Rn
--[[15:4]]
	local r4 = d3i3
	do return r4 end
--[[stream end]]
		end --[[function]]
	d2i7 = r4
-- Rf2RnRnn
--[[18:4]]
	local d2i8
	local r5 = function(d3i1, d3i2)
--[[stream start]]
	local d3i3
--[[20:7]]
	local r1 = d3i1
	local r2 = d3i2
	local r3 = r1 + r2
-- Rn
	d3i1 = r3
-- Rn
	d3i3 = r3
-- Rn
--[[21:4]]
	local r4 = d3i3
	local r5 = d3i1
	do return r4, r5 end
--[[stream end]]
		end --[[function]]
	d2i8 = r5
--[[24:4]]
	local d2i9
	local r6 = function(d3i1, d3i2)
--[[stream start]]
	local d3i3
--[[26:7]]
	local r1 = d2i8
	local r2 = d3i1
	local r3 = d3i2
	local r4, r5 = r1(r2, r3)
	d3i1 = r5
-- Rn
	d3i3 = r4
-- Rn
--[[27:4]]
	local r6 = d3i3
	local r7 = d3i1
	do return r6, r7 end
--[[stream end]]
		end --[[function]]
	d2i9 = r6
--[[29:4]]
	local r7 = d2i9
	local r8 = d2i4
	local r9 = 10
	local r10, r11 = r7(r8, r9)
	d2i4 = r11
-- Rn
--[[31:4]]
	local d2i10
--[[32:4]]
	local r12 = d2i8
	d2i10 = r12
-- Rf2BnRnn
--[[stream end]]