local modname = ...
local object = require('base.object')
local control = object:module(modname)
local controlset
local position

--[[	* @class gi.control @extends base.object
		* A base class for data-driven UI widgets, which can respond to user input.
		* @property position - rectangle which bounds the control relative to the parent, entire area by default
		* @property name - identifies the control withing the closest named parent
		* @property @readonly fullname - combination of all named parents' names with this control's name, used to obtain the control from gifield
		* @property @readonly parent
]]

--[[	* @method createcontrol
		* Constructs a new control from the property table.
		* @arg it - a table defining new control's properties
		* @arg parent
		* @return new control
]]
function control:createcontrol(it, parent, prefix)
	if not controlset then
		controlset = require('gi.controlset')
	end
	local class = assert(it.class and controlset[it.class])
	if class then
		return class:create(it, parent, prefix)
	end
end

function control:init(it, parent, prefix)
	prefix = prefix or ''
	self.parent = parent
	self.position = position:create(it, parent and parent.position, prefix .. 'position_')
	self.clipregion = position:create(it, parent and parent.position, prefix .. 'clipregion_')
	self.name = it[prefix .. 'name']
	self.gifield = parent and parent.gifield
	if self.name then
		if parent and parent.name then
			self.fullname = parent.name .. '.' .. self.name
		else
			self.fullname = self.name
		end
		self.gifield.controls[self.fullname] = self
	end
	if parent then
		parent:insertcontrol(self)
	end
end

function control:release()
	if parent then
		parent:removecontrol(self)
	end
end

--[[	* @method setactive
		* Changes the control's status. Inactive controls are invisible and ignore all user input.
		* @arg active
]]
function control:setactive(active)
end

function control:insertcontrol(ic)
	if parent then
		parent:insertcontrol(self)
	end
end

function control:removecontrol(ic)
	if parent then
		parent:removecontrol(self)
	end
end

function control:handle(message, ...)
	-- if message ~= 'tick' and self._NAME ~= 'gi.gifield' then
		-- print(self, self._NAME, message, ...)
	-- end
	local messagehandler = self['handle_' .. message]
	if messagehandler then
		messagehandler(self, message, ...)
	end
end

--[[	* @method updateposition
		* Updates owned objects and data to realize current position values.
]]
function control:updateposition()
	self.position:update()
	self.clipregion:update()
end

position = require('gi.position')
