local modname = ...
local control = require('gi.control')
local linearray = control:module(modname)
local labelbase = require('gi.labelbase')

function linearray:init(it, parent)
	control.init(self, it, parent)
	local lineheight = it.lineheight or 16
	local linecount = math.floor(self.position.height / lineheight)
	self.labels = {}
	for i = 1, linecount do
		local lit = {
			basex = self.position.left,
			basey = self.position.bottom + (i - 1) * lineheight,
			fontname = it.fontname,
			color = it.color,
			text = it.lines and it.lines[i],
		}
		self.labels[i] = labelbase:create(lit, self)
	end
end

function linearray:release()
	for i, label in ipairs(self.labels) do
		label:release()
	end
end

function linearray:setline(i, text)
	if self.labels[i] then
		self.labels[i]:settext(text)
	end
end

function linearray:getline(i)
	if self.labels[i] then
		return self.labels[i]:gettext(text)
	end
end
