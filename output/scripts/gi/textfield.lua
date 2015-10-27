local modname = ...
local object = require('base.object')
local textfield = object:module(modname)
local ffi = require('ffi')
local resourceset = require('rs.resourceset')
local stringinstance = require('host.stringinstance')
local statictexture = require('host.statictexture')

local tfplane = object:derive({}, modname..'.tfplane')

function tfplane:init(tf)
	self.tf = tf
	self.bitmap = ffi.new('uint32_t[?]', tf.width * tf.height)
	self.texture = statictexture:create()
	local bottom = 0
	local top = tf.font.height
	while top < tf.height do
		local field = {
			plane = self,
			x = 0,
			y = bottom,
			width = tf.width,
			height = tf.font.height,
			next = tf.field,
		}
		tf.field = field
		bottom = top
		top = top + tf.font.height
	end
end

function tfplane:update()
	local datasize = self.tf.width * self.tf.height * 4
	local si = stringinstance:create(datasize, datasize, ffi.cast('char const*', self.bitmap))
	self.texture:assign(si, self.tf.width, self.tf.height)
	si:release()
end

function textfield:init(fontname)
	self.font = resourceset:loadresource(fontname, 'Bitfont'):getfont()
	self.planes = {}
	self.width = 1024
	self.height = 32
end

function textfield:allocfield()
	if not self.field then
		self.planes[#self.planes + 1] = tfplane:create(self)
	end
	local field = self.field
	self.field = field.next
	field.next = nil
	return field
end

function textfield:releasefield(field)
	field.next = self.field
	self.field = field
end

function textfield:render(field, line)
	line = line or ''
	local rir = self.font:getlinedims(line)
	if rir.width > field.width or rir.height > field.height then
		print(rir.height)
		return
	end
	local plane = field.plane
	for y = field.y, field.y + field.height - 1 do
		local row = plane.bitmap + (self.width * y)
		ffi.C.memset(row, 0, field.width * 4)
	end
	rir.basex = field.x + rir.underhang
	rir.basey = field.y + rir.descent
	rir.bitmap = plane.bitmap
	rir.stride = self.width
	rir.value = 0xffffffff
	rir.mask = 1
	self.font:render(line, rir)
	plane:update()
	local posinfo = {
		x1 = field.x / self.width,
		y1 = field.y / self.height,
		x2 = (field.x + rir.width) / self.width,
		y2 = (field.y + rir.height) / self.height,
		dx = rir.underhang,
		dy = rir.descent,
		width = rir.width,
		height = rir.height,
		texture = plane.texture,
	}
	return posinfo
end
