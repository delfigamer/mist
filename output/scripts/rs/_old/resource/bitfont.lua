local modname = ...
local base = require('rs.resource.base')
local bitfont = base:module(modname, 'Bitfont')
local bitfontformat = require('rs.format.bitfont')
local exttable = require('rs.resource.exttable')
local resourceset = require('rs.resourceset')

bitfont.isfontresource = true
bitfont.searchpath = '?.bitfont'

function bitfont:createnew()
	error('cannot create font resources')
end

function bitfont:afterloading(task)
	local fb = self.metadata.fallback
	if fb then
		local res = resourceset:loadresource(fb, 'Bitfont')
		self.fallback = res
		task:yield{
			subtask = res.loading}
		self.bitfont:setfallback(self.fallback:getfont())
	end
end

function bitfont:getfont()
	self:finishloading()
	return self.bitfont
end
