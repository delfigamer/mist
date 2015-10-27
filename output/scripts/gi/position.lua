local modname = ...
local object = require('base.object')
local position = object:module(modname)
local info = require('host.info')

local function determineposition(ileft, iwidth, iright, pwidth)
	if ileft and iwidth then
		return ileft, iwidth
	elseif iright and iwidth then
		return pwidth + iright - iwidth, iwidth
	elseif ileft and iright then
		return ileft, pwidth + iright - ileft
	elseif ileft then
		return ileft, pwidth - ileft
	elseif iwidth then
		return 0, pwidth
	elseif iright then
		return 0, pwidth + iright
	else
		return 0, pwidth
	end
end

function position:init(it, parent, prefix)
	prefix = prefix or ''
	self.parent = parent
	if it then
		self.left = it[prefix .. 'left']
		self.width = it[prefix .. 'width']
		self.right = it[prefix .. 'right']
		self.bottom = it[prefix .. 'bottom']
		self.height = it[prefix .. 'height']
		self.top = it[prefix .. 'top']
	end
	self:update()
end

function position:update()
	local pleft, pbottom, pwidth, pheight
	if self.parent then
		pleft, pbottom, pwidth, pheight = self.parent:getglobalpos()
	else
		pleft, pbottom, pwidth, pheight = 0, 0, info.width, info.height
	end
	self.local_left, self.local_width = determineposition(self.left, self.width, self.right, pwidth)
	self.local_bottom, self.local_height = determineposition(self.bottom, self.height, self.top, pheight)
	self.local_right = self.local_left + self.local_width - pwidth
	self.local_top = self.local_bottom + self.local_height - pheight
	self.global_left = self.local_left + pleft
	self.global_bottom = self.local_bottom + pbottom
	self.global_width = self.local_width
	self.global_height = self.local_height
	self.global_right = self.global_left + self.global_width
	self.global_top = self.global_bottom + self.global_height
end

function position:getlocalpos()
	return
		self.local_left, self.local_bottom,
		self.local_width, self.local_height,
		self.local_right, self.local_top
end

function position:getglobalpos()
	return
		self.global_left, self.global_bottom,
		self.global_width, self.global_height,
		self.global_right, self.global_top
end

function position:getsize()
	return
		self.local_width, self.local_height
end

function position:inside(x, y)
	local left, bottom, width, height = self:getlocalpos()
	x = x - left
	y = y - bottom
	if x >= 0 and x < width and y >= 0 and y < height then
		return x, y
	end
end

function position:clipvalues(region_left, region_bottom, region_width, region_height)
	local region_right = region_left + region_width
	local region_top = region_bottom + region_height
	local clip_left, clip_bottom, clip_right, clip_top
	local
		global_left, global_bottom,
		global_width, global_height,
		global_right, global_top = self:getglobalpos()
	if region_left < global_left then
		clip_left = global_left - region_left
	else
		clip_left = 0
	end
	if region_bottom < global_bottom then
		clip_bottom = global_bottom - region_bottom
	else
		clip_bottom = 0
	end
	if region_right > global_right then
		clip_right = global_right - region_left
	else
		clip_right = region_width
	end
	if region_top > global_top then
		clip_top = global_top - region_bottom
	else
		clip_top = region_height
	end
	return clip_left, clip_bottom, clip_right, clip_top
end
