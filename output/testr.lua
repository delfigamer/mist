--[[stream start]]
--[[scripts/test.exl]]
--[[1:4]]
	local r1 = "foobar"
	local d3i1 = r1
--[[2:4]]
	local r2 = 42
	local d3i2 = r2
--[[3:4]]
	local d3i3 = r0
--[[4:4]]
	local d3i4 = r0
--[[5:4]]
	local r3 = 44
	d3i4 = r3
-- Rn
--[[6:4]]
	local d3i5 = r0
--[[10:4]]
	local d3i6 = r0
--[[11:4]]
	local d3i7 = r0
--[[12:4]]
	local r4 = function(d4i1, d4i2)
--[[stream start]]
	local d4i3 = r0
--[[scripts/test.exl]]
--[[14:7]]
	r1 = d4i1
	r2 = d4i2
	local r3 = r1 + r2
-- Rn
	d4i3 = r3
-- Rn
--[[file end]]
--[[15:4]]
	r4 = d4i3
	do return r4 end
--[[stream end]]
		end --[[function]]
	d3i7 = r4
-- Rf2RnRnn
--[[18:4]]
	local d3i8 = r0
	local r5 = function(d4i1, d4i2)
--[[stream start]]
	local d4i3 = r0
--[[scripts/test.exl]]
--[[20:7]]
	r1 = d4i1
	r2 = d4i2
	local r3 = r1 + r2
-- Rn
	d4i1 = r3
-- Rn
	d4i3 = r3
-- Rn
--[[file end]]
--[[21:4]]
	r4 = d4i3
	r5 = d4i1
	do return r4, r5 end
--[[stream end]]
		end --[[function]]
	d3i8 = r5
--[[24:4]]
	local d3i9 = r0
	local r6 = function(d4i1, d4i2)
--[[stream start]]
	local d4i3 = r0
--[[scripts/test.exl]]
--[[26:7]]
	r1 = d3i8
	r2 = d4i1
	r3 = d4i2
	local r4, r5 = r1(r2, r3)
	d4i1 = r5
-- Rn
	d4i3 = r4
-- Rn
--[[file end]]
--[[27:4]]
	r6 = d4i3
	r7 = d4i1
	do return r6, r7 end
--[[stream end]]
		end --[[function]]
	d3i9 = r6
--[[29:4]]
	r7 = d3i9
	r8 = d3i4
	local r9 = 10
	local r10, r11 = r7(r8, r9)
	d3i4 = r11
-- Rn
--[[31:4]]
	local d3i10 = r0
--[[32:4]]
	r12 = d3i8
	d3i10 = r12
-- Rf2BnRnn
--[[34:4]]
	local d3i11 = r0
--[[35:4]]
	local r13 = {
		}
-- R4test_7__classc
	d3i11 = r13
-- R4test_7__classc
--[[37:4]]
	local d3i12 = r0
	local r14 = function(d4i1, d4i2)
--[[stream start]]
	local d4i3 = r0
--[[scripts/test.exl]]
--[[file end]]
--[[38:4]]
	r1 = d4i3
	r2 = d4i2
	do return r1, r2 end
--[[stream end]]
		end --[[function]]
	d3i12 = r14
--[[39:4]]
	local d3i13 = r0
--[[40:4]]
	r15 = d3i12
	d3i13 = r15
-- Rf2R4test_7__classcB4test_7__classc4test_7__classc
--[[file end]]
--[[stream end]]