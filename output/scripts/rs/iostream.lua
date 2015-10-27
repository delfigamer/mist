local modname = ...
local object = require('base.object')
local iostream = object:module(modname)
local dword = require('host.types').dword
local ffi = require('ffi')
local host = require('host')
local uint64 = require('rs.scalars').uint64

local function align8(pos)
	local dw = dword(pos)
	if bit.band(dw.l, 7) == 0 then
		return pos
	end
	if bit.band(dw.l, 0xfffffff8) == bit.tobit(0xfffffff8) then
		dw.h = dw.h + 1
		dw.l = 0
		return dw.d
	else
		dw.l = bit.band(dw.l, 0xfffffff8) + 8
		return dw.d
	end
end

local function trimtolimit(length, position, limit)
	if limit and position + length > limit then
		length = tonumber(limit - position)
		if length > 0 then
			return length
		else
			return 0
		end
	else
		return length
	end
end

function iostream:init(io, position)
	self.io = io
	self.position = position and (position + 0ULL) or 0ULL
	self.substack = {}
end

function iostream:action(iomethod, length, buffer)
	length = trimtolimit(length, self.position, self.limit)
	local result = self.io[iomethod](self.io, self.position, length, buffer)
	self.position = self.position + result
	return result
end

function iostream:read(length, buffer)
	return self:action('ioread', length, buffer)
end

function iostream:write(length, buffer)
	return self:action('iowrite', length, buffer)
end

function iostream:readsubstream()
	self.position = align8(self.position)
	local length = uint64:read(self)
	local limit = self.position + length
	local entry = {
		parentlimit = self.limit,
	}
	if not self.limit or limit < self.limit then
		self.limit = limit
	end
	table.append(self.substack, entry)
	return self
end

function iostream:writesubstream()
	local entry = {
		write = true,
		start = align8(self.position),
	}
	table.append(self.substack, entry)
	self.position = entry.start + 8
	return self
end

function iostream:skipsubstream()
	self.position = align8(self.position)
	local length = uint64:read(self)
	self.position = align8(self.position + length)
end

function iostream:close()
	local entry = table.pop(self.substack)
	if entry then
		if entry.write then
			local position = self.position
			self.position = entry.start
			uint64:write(position - entry.start - 8, self)
			self.position = align8(position)
		else
			self.position = align8(self.limit)
			self.limit = entry.parentlimit
		end
	else
		self.io:setiosize(self.position)
	end
end

function iostream:skip(length)
	length = trimtolimit(length, self.position, self.limit)
	self.position = self.position + length
	return length
end
