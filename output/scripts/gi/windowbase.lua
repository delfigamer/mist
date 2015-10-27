local modname = ...
local control = require('gi.control')
local windowbase = control:module(modname)
local set

--[[	* @class gi.windowbase @extends gi.control
		* A base class for controls which contain other controls.
		* @property innercontrols
]]

function windowbase:init(it, parent)
	control.init(self, it, parent)
	self.innercontrols = set:create()
	self.isactive = true
	self.pdcontrols = {}
end

function windowbase:release()
end

function windowbase:insertcontrol(ic)
	self.innercontrols:insert(ic)
end

function windowbase:removecontrol(ic)
	self.innercontrols:remove(ic)
end

function windowbase:updateposition()
	self.position:update()
	for i, control in self.innercontrols:pairs() do
		control:updateposition()
	end
end

function windowbase:setactive(active)
	self.isactive = active
	self.shapegroup:setactive(active)
	for i, control in self.controls:pairs() do
		control:setactive(active)
	end
end

function windowbase.trace_iterate(state, i)
	for i, control in state.func, state.state, i do
		local cx, cy = control.position:inside(state.x, state.y)
		if cx then
			return i, control, cx, cy
		end
	end
end

--[[	* @method_iterator trace
		* Iterates through controls overlapping a specified point relative to this window. For each one, the point's coordinates relative to the current control is also returned.
		* @arg x
		* @arg y
		* @return index of a current control in the set
		* @return control itself
		* @return inner x
		* @return inner y
]]
function windowbase:trace(x, y)
	local func, state, i = self.innercontrols:pairs()
	return self.trace_iterate, {func = func, state = state, x = x, y = y}, i
end

function windowbase:handle_pointmove(message, point, x, y)
	if not self.isactive then
		return
	end
	local pd = table.provide(self.pdcontrols, point)
	for control, value in pairs(pd) do
		if bit.band(value, 1) ~= 0 and not control.position:inside(x, y) then
			control:handle('pointleave', point)
			pd[control] = bit.band(value, bit.bnot(1))
		end
	end
	for i, control, cx, cy in self:trace(x, y) do
		local pdvalue = pd[control] or 0
		if bit.band(pdvalue, 1) == 0 then
			control:handle('pointenter', point, cx, cy)
			pd[control] = bit.bor(pdvalue, 1)
		end
		control:handle('pointmove', point, cx, cy)
	end
end

function windowbase:handle_pointdown(message, point, x, y)
	if not self.isactive or point == 0 then
		return
	end
	local pd = table.provide(self.pdcontrols, point)
	for i, control, cx, cy in self:trace(x, y) do
		local pdvalue = pd[control] or 0
		control:handle('pointdown', point, cx, cy)
		pd[control] = bit.bor(pdvalue, 2)
	end
end

function windowbase:handle_pointenter(message, point, x, y)
	if not self.isactive then
		return
	end
	local pd = table.provide(self.pdcontrols, point)
	for i, control, cx, cy in self:trace(x, y) do
		local pdvalue = pd[control] or 0
		if bit.band(pdvalue, 1) == 0 then
			control:handle('pointenter', point, cx, cy)
		end
		pd[control] = bit.bor(pdvalue, 1)
	end
end

function windowbase:handle_pointleave(message, point)
	if not self.isactive then
		return
	end
	local pd = table.provide(self.pdcontrols, point)
	for control, pdvalue in pairs(pd) do
		if bit.band(pdvalue, 1) ~= 0 then
			control:handle('pointleave', point)
			pd[control] = bit.band(pdvalue, bit.bnot(1))
		end
	end
end

function windowbase:handle_pointup(message, point)
	if not self.isactive or point == 0 then
		return
	end
	local pd = table.provide(self.pdcontrols, point)
	for control, pdvalue in pairs(pd) do
		if bit.band(pdvalue, 2) ~= 0 then
			control:handle('pointup', point)
		end
	end
	self.pdcontrols[point] = nil
end

set = require('base.set')
