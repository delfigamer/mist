local modname = ...
local windowbase = require('gi.windowbase')
local buttonrow = windowbase:module(modname)
local button

--[[	* @class gi.buttonrow @extends gi.windowbase
		* A vertical or horizontal row of tightly packed buttons.
		* Used as a base class for different kinds of menus.
		* @property buttonbase - common properties of contained buttons
		* @property direction - 'right', 'left', 'up' or 'down' (default)
		* @property @initonly buttons - a list of items to spawn, in format: {{name1, label1}, {name2, label2}}
		* @property @initonly spacing - additional space added to buttons' adjusted size
]]

local directionmode = {
	right = {},
	-- left = {},
	-- up = {},
	down = {},
}

function directionmode.right.init(lit)
	lit.position_left = 0
end

function directionmode.right.adjust(adjust, lit, button)
	if adjust and button.adjustwidth then
		button:adjustwidth()
	end
	local width, height = button.position:getsize()
	lit.position_left = lit.position_left + width
end


function directionmode.down.init(lit)
	lit.position_top = 0
end

function directionmode.down.adjust(adjust, lit, button)
	if adjust and button.adjustheight then
		button:adjustheight()
	end
	local width, height = button.position:getsize()
	lit.position_top = lit.position_top - height
end


function buttonrow:init(it, parent, prefix)
	prefix = prefix or ''
	windowbase.init(self, it, parent, prefix)
	local direction = it[prefix .. 'direction']
	local dmode = direction and directionmode[direction] or directionmode.down
	local adjust = it[prefix .. 'adjust']
	if adjust == nil then
		adjust = true
	end
	local buttons = it[prefix .. 'buttons']
	if buttons then
		local lit = {}
		dmode.init(lit)
		for i, item in ipairs(buttons) do
			setmetatable(lit, {__index = item})
			local button = self:createcontrol(lit, self)
			dmode.adjust(adjust, lit, button)
		end
	end
end
