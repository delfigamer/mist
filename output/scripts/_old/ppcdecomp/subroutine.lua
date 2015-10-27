local modname = ...
local object = require('base.object')
local subroutine = object:module(modname)
local trace = require('ppcdecomp.trace')
local dbit = require('ppcdecomp.bit')

function subroutine:init(module, address)
	self.module = module
	self.address = address
	self.name = module.conf.subroutine_name[address] or ''
	self.traces = {}
	self.subcalls = {}
	self.callers = {}
end

function subroutine:walk()
-- 	print(string.format('walking subroutine %#10x %s', self.address, self.name))
	self.pending = {}
	self.start = self:trace(self.address)
	while true do
		local ia, trace = next(self.pending)
		if ia then
			trace:walk()
			self.pending[ia] = nil
		else
			break
		end
	end
	self.pending = nil
end

function subroutine:trace(first)
	if self.traces[first] then
		return self.traces[first]
	else
		local trace = trace:create(self, first)
		self.pending[first] = trace
		self.traces[first] = trace
		return trace
	end
end

function subroutine:subcall(address)
	local sc = self.module:subroutine(address)
	self.subcalls[address] = sc
	sc.callers[self.address] = self
	return sc
end

function subroutine:funclist_defstring(lp, list, format)
	local lines = {}
	local i = 0
	for k, v in pairs(list) do
		i = i + 1
		lines[i] = v
	end
	table.sort(lines, function(l,r) return l.address < r.address end)
	for i, v in ipairs(lines) do
		lines[i] = format(lp, v)
	end
	return table.concat(lines, '\n')
end

local function subcall_format(lp, v)
	return string.format('%s-> %#10x %s', lp, v.address, v.name)
end

function subroutine:subcalls_defstring(lp)
	return self:funclist_defstring(lp, self.subcalls, subcall_format)
end

local function caller_format(lp, v)
	return string.format('%s<- %#10x %s', lp, v.address, v.name)
end

function subroutine:callers_defstring(lp)
	return self:funclist_defstring(lp, self.callers, caller_format)
end

function subroutine:traces_defstring(lp)
	local lines = {}
	local i = 0
	for k, v in pairs(self.traces) do
		i = i + 1
		lines[i] = v
	end
	table.sort(lines, function(l,r) return l.first < r.first end)
	for i, v in ipairs(lines) do
		lines[i] = string.format('%s%s', lp, v:defstring(lp .. '\t'))
	end
	return table.concat(lines, '\n')
end

function subroutine:defstring(lp)
	return string.format('%#x %s:\n%s\n%s\n%s', self.address, self.name,
		self:subcalls_defstring(lp),
		self:callers_defstring(lp),
		self:traces_defstring(lp))
end
