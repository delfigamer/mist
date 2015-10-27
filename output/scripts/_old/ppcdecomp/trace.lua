local modname = ...
local object = require('base.object')
local trace = object:module(modname)
local dbit = require('ppcdecomp.bit')

local function isbranch(i)
	return
		dbit.opcd(i) == 16 or
		dbit.opcd(i) == 18 or
		(dbit.opcd(i) == 19 and (dbit.xxo(i) == 16 or dbit.xxo(i) == 528))
end

local function constcondition(i)
	return dbit.bitfield(i, 6 + 0) and dbit.bitfield(i, 6 + 2)
end

local function nextstart(i, iea)
	local lk = dbit.bitfield(i, 31)
	if dbit.opcd(i) == 16 then
		local tea = dbit.exts(bit.lshift(dbit.bitfield(i, 16, 30), 2), 16)
		local aa = dbit.bitfield(i, 30)
		if not aa then
			tea = tea + iea
		end
		if lk then
			return iea + 4, nil, tea
		else
			if constcondition(i) then
				return tea
			else
				return iea + 4, tea
			end
		end
	elseif dbit.opcd(i) == 18 then
		local tea = dbit.exts(bit.lshift(dbit.bitfield(i, 6, 30), 2), 26)
		local aa = dbit.bitfield(i, 30)
		if not aa then
			tea = tea + iea
		end
		if lk then
			return iea + 4, nil, tea
		else
			return tea
		end
	elseif dbit.opcd(i) == 19 then
		local xo = dbit.xxo(i)
		local spr = xo == 16 and 'lr' or 'ctr'
		if lk then
			return iea + 4, nil, nil, spr
		else
			if constcondition(i) then
				return nil, nil, nil, spr
			else
				return iea + 4, nil, nil, spr
			end
		end
	end
end

function trace:init(subroutine, first)
	self.subroutine = subroutine
	self.module = subroutine.module
	self.first = first
	self.last = -4
end

function trace:walk()
-- 	print(string.format('\twalking trace 0x%x', self.first))
	local ea = self.first
	while true do
		local inst = self.module:word(ea)
		if isbranch(inst) then
			self.last = ea
			local next_trace_1, next_trace_2, called_subroutine, spr_target = nextstart(inst, ea)
			self.branch = {}
			if next_trace_1 then
				self.branch[1] = self.subroutine:trace(next_trace_1)
			end
			if next_trace_2 then
				self.branch[2] = self.subroutine:trace(next_trace_2)
			end
			if called_subroutine then
				self.subcall = self.subroutine:subcall(called_subroutine)
			end
			if spr_target then
				self.sprtarget = spr_target
			end
			break
		end
		ea = ea + 4
	end
end

function trace:defstring(lp)
	return string.format(
		'%#x -> %10s %10s %3s %s',
		self.first,
		self.branch[1] and string.format('%#x', self.branch[1].first) or '',
		self.branch[2] and string.format('%#x', self.branch[2].first) or '',
		self.sprtarget or '',
		self.subcall and string.format('-> %#x %s', self.subcall.address, self.subcall.name) or '')
end
