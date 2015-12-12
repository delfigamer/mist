local modname = ...
local object = require('exl.object')
local fulltype = object:module(modname)
local common

function fulltype:init(ti, lvalue, rvalue)
	self.ti = ti
	self.lvalue = lvalue
	self.rvalue = rvalue
end

function fulltype:getserial()
	local am
	if self.lvalue and self.rvalue then
		am = 'B'
	elseif self.lvalue and not self.rvalue then
		am = 'L'
	elseif not self.lvalue and self.rvalue then
		am = 'R'
	else
		return 'N'
	end
	if self.ti then
		local tis = self.ti:getserial()
		if tis then
			return am .. tis
		else
			return
		end
	else
		return am
	end
end

function fulltype:defstring(lp)
	local s = self:getserial()
	if s then
		return s
	end
	local am
	if self.lvalue and self.rvalue then
		am = 'RL'
	elseif self.lvalue and not self.rvalue then
		am = 'L'
	elseif not self.lvalue and self.rvalue then
		am = 'R'
	else
		return 'N'
	end
	return string.format('%s %s',
		am,
		common.defstring(self.ti, lp))
end

common = require('exl.common')