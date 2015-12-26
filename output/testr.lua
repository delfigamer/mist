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
	local r4 = require("base.object"):derive(nil, "test.classc")
	local d3i5 = r4
--[[scripts/test.exl]]
--[[7:7]]
--[[8:7]]
--[[file end]]
--[[10:4]]
	local d3i6 = r0
--[[11:4]]
	local d3i7 = r0
--[[12:4]]
	local r5 = function(d4i1, d4i2)
--[[stream start]]
	local d4i3 = r0
--[[scripts/test.exl]]
--[[14:7]]
	local r1 = d4i1
	local r2 = d4i2
	local r3 = r1 + r2
-- Rn
	d4i3 = r3
-- Rn
--[[file end]]
--[[15:4]]
	local r4 = d4i3
	do return r4 end
--[[stream end]]
		end --[[function]]
	d3i7 = r5
-- Rf2RnRnn
--[[18:4]]
	local d3i8 = r0
	local r6 = function(d4i1, d4i2)
--[[stream start]]
	local d4i3 = r0
--[[scripts/test.exl]]
--[[20:7]]
	local r1 = d4i1
	local r2 = d4i2
	local r3 = r1 + r2
-- Rn
	d4i1 = r3
-- Rn
	d4i3 = r3
-- Rn
--[[file end]]
--[[21:4]]
	local r4 = d4i3
	local r5 = d4i1
	do return r4, r5 end
--[[stream end]]
		end --[[function]]
	d3i8 = r6
--[[24:4]]
	local d3i9 = r0
	local r7 = function(d4i1, d4i2)
--[[stream start]]
	local d4i3 = r0
--[[scripts/test.exl]]
--[[26:7]]
	local r1 = d3i8
	local r2 = d4i1
	local r3 = d4i2
	local r4, r5 = r1(r2, r3)
	d4i1 = r5
-- Rn
	d4i3 = r4
-- Rn
--[[file end]]
--[[27:4]]
	local r6 = d4i3
	local r7 = d4i1
	do return r6, r7 end
--[[stream end]]
		end --[[function]]
	d3i9 = r7
--[[29:4]]
	local r8 = d3i9
	local r9 = d3i4
	local r10 = 10
	local r11, r12 = r8(r9, r10)
	d3i4 = r12
-- Rn
--[[31:4]]
	local d3i10 = r0
--[[32:4]]
	local r13 = d3i8
	d3i10 = r13
-- Rf2BnRnn
--[[34:4]]
	local d3i11 = r0
--[[35:4]]
	local r14 = d3i5
	local r15 = r14:new()
-- R4test_6classc
	d3i11 = r15
-- R4test_6classc
--[[37:4]]
	local d3i12 = r0
	local r16 = function(d4i1, d4i2)
--[[stream start]]
	local d4i3 = r0
--[[scripts/test.exl]]
--[[file end]]
--[[38:4]]
	local r1 = d4i3
	local r2 = d4i2
	do return r1, r2 end
--[[stream end]]
		end --[[function]]
	d3i12 = r16
--[[39:4]]
	local d3i13 = r0
--[[40:4]]
	local r17 = d3i12
	d3i13 = r17
-- Rf2R4test_6classcB4test_6classc4test_6classc
--[[42:4]]
	local d3i14 = r0
	local r18 = function(d4i1, d4i2, d4i3)
--[[stream start]]
	local d4i4 = r0
--[[scripts/test.exl]]
--[[43:7]]
	local r1 = d4i1
	local r2 = r1:new()
-- R4test_6classc
	d4i4 = r2
-- R4test_6classc
--[[file end]]
--[[44:4]]
	local r3 = d4i4
	do return r3 end
--[[stream end]]
		end --[[function]]
	d3i14 = r18
--[[45:4]]
	local r19 = d3i14
	local r20 = d3i5
	local r21 = 10
	local r22 = 20
	local r23 = r19(r20, r21, r22)
-- R4test_6classc
	d3i11 = r23
-- R4test_6classc
--[[47:4]]
	local d3i15 = r0
	local r24 = function(d4i1)
--[[stream start]]
	local d4i2 = r0
--[[scripts/test.exl]]
--[[file end]]
--[[48:4]]
	local r1 = d4i2
	do return r1 end
--[[stream end]]
		end --[[function]]
	d3i15 = r24
--[[50:4]]
	local r25 = d3i15
	local r26 = d3i15
	local r27 = d3i15
	local r28 = 42
	local r29 = r27(r28)
-- Rn
	local r30 = r26(r29)
-- Rn
	local r31 = r25(r30)
-- Rn
--[[file end]]
--[[stream end]]