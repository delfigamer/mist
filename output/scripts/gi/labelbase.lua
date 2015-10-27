local modname = ...
local control = require('gi.control')
local labelbase = control:module(modname)
local types = require('host.types')

local function colortype(a, ...)
	if a then
		return types.colortype(a, ...)
	end
end

labelbase.color = colortype(1, 1, 1, 1)
labelbase.align = 0
labelbase.valign = 0
labelbase.clip = false

function labelbase:init(it, parent, prefix)
	prefix = prefix or ''
	control.init(self, it, parent, prefix)
	self.textfield = self.gifield.textfieldset:gettf(it[prefix .. 'fontname'] or '')
	self.tffield = self.textfield:allocfield()
	self.color = colortype(it[prefix .. 'color'])
	self.align = it[prefix .. 'align']
	self.valign = it[prefix .. 'valign']
	self.clip = it[prefix .. 'clip']
	self.fitwidth = it[prefix .. 'fitwidth']
	self.fitheight = it[prefix .. 'fitheight']
	local text = it[prefix .. 'text']
	if text then
		self:settext(text)
	end
end

function labelbase:release()
	self.textfield:releasefield(self.tffield)
end

function labelbase:settext(line)
	self.posinfo = self.textfield:render(self.tffield, line)
	if self.posinfo then
		if (self.texture and self.texture.ptr) ~= (self.posinfo.texture and self.posinfo.texture.ptr) then
			if self.sprite then
				self.sprite:release()
			end
			self.texture = self.posinfo.texture
			self.spritefield = self.gifield.textmultisf:getruntimeentry(self.posinfo.texture).spritefield
			self.sprite = self.spritefield:newsprite()
			self.sprite:setcolor(self.color)
		end
		self:updateposition()
		self.text = line
	end
end

function labelbase:updateposition()
	self.position:update()
	if self.posinfo then
		local left, bottom, width, height, right, top = self.position:getglobalpos()
		local origin_x = left + math.floor((width - self.posinfo.width)*self.align)
		local origin_y = bottom + math.floor((height - self.posinfo.height)*self.valign)
		self.basex = origin_x + self.posinfo.dx
		self.basey = origin_y + self.posinfo.dy
		local sprite_left, sprite_bottom, sprite_width, sprite_height
		local tex_left, tex_bottom, tex_right, tex_top
		if self.clip then
			local factor_left, factor_bottom, factor_right, factor_top
			if origin_x < left then
				factor_left = (left - origin_x) / self.posinfo.width
			else
				factor_left = 0
			end
			if origin_y < bottom then
				factor_bottom = (bottom - origin_y) / self.posinfo.height
			else
				factor_bottom = 0
			end
			if origin_x + self.posinfo.width > right then
				factor_right = (right - origin_x) / self.posinfo.width
			else
				factor_right = 1
			end
			if origin_y + self.posinfo.height > top then
				factor_top = (top - origin_y) / self.posinfo.height
			else
				factor_top = 1
			end
			sprite_left = origin_x + self.posinfo.width*factor_left
			sprite_bottom = origin_y + self.posinfo.height*factor_bottom
			sprite_width = self.posinfo.width*(factor_right-factor_left)
			sprite_height = self.posinfo.height*(factor_top-factor_bottom)
			local base_tex_dx = self.posinfo.x2 - self.posinfo.x1
			local base_tex_dy = self.posinfo.y2 - self.posinfo.y1
			tex_left = self.posinfo.x1 + base_tex_dx*factor_left
			tex_bottom = self.posinfo.y1 + base_tex_dy*factor_bottom
			tex_right = self.posinfo.x1 + base_tex_dx*factor_right
			tex_top = self.posinfo.y1 + base_tex_dy*factor_top
		else
			sprite_left = origin_x
			sprite_bottom = origin_y
			sprite_width = self.posinfo.width
			sprite_height = self.posinfo.height
			tex_left = self.posinfo.x1
			tex_bottom = self.posinfo.y1
			tex_right = self.posinfo.x2
			tex_top = self.posinfo.y2
		end
		self.sprite:setposition(sprite_left + sprite_width/2, sprite_bottom + sprite_height/2)
		self.sprite:setextent(sprite_width/2, 0, 0, sprite_height/2)
		self.sprite:settexregion1(tex_left, tex_bottom, tex_right, tex_top)
		self.render_left = sprite_left
		self.render_bottom = sprite_bottom
		self.render_width = sprite_width
		self.render_height = sprite_height
		self.render_right = sprite_left + sprite_width
		self.render_top = sprite_bottom + sprite_height
		self.local_left = self.render_left - left
		self.local_bottom = self.render_bottom - bottom
		self.local_width = self.render_width
		self.local_height = self.render_height
		self.local_right = self.render_right - right
		self.local_top = self.render_top - top
	end
end

function labelbase:gettext()
	return self.text
end
