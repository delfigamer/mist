local modname = ...
local object = require('base.object')
local module = object:module(modname)
local subroutine = require('ppcdecomp.subroutine')
local dbit = require('ppcdecomp.bit')

function module:init(sourcedata, conf)
	self.sourcedata = sourcedata
	self.conf = conf
	self.subroutines = {}
end

function module:buildcalltree()
	self.pending = {}
	self.root = self:subroutine(self.conf.startia)
	while true do
		local ia, subroutine = next(self.pending)
		if ia then
			subroutine:walk()
			self.pending[ia] = nil
		else
			break
		end
	end
end

function module:word(ea)
	return dbit.bew(self.sourcedata, self.conf.offset + ea + 1)
end

function module:subroutine(address)
	if self.subroutines[address] then
		return self.subroutines[address]
	else
		local subroutine = subroutine:create(self, address)
		self.pending[address] = subroutine
		self.subroutines[address] = subroutine
		return subroutine
	end
end

function module:defstring(lp)
	local lines = {}
	local i = 0
	for k, v in pairs(self.subroutines) do
		i = i + 1
		lines[i] = v
	end
	table.sort(lines, function(l,r) return l.address < r.address end)
	for i, v in ipairs(lines) do
		lines[i] = lp .. lines[i]:defstring(lp .. '\t')
	end
	return table.concat(lines, '\n\n')
end
