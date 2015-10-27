local modname = ...
module(modname, package.seeall)
local ffi = require('ffi')
local object = require('base.object')

limits = object:derive({})

function limits:init(size)
	self.size = size
	self.mins = ffi.new('float[?]', size + 1)
	self.maxs = ffi.new('float[?]', size + 1)
	for i = 1, size do
		self.mins[i] = math.huge
		self.maxs[i] = -math.huge
	end
end

function limits:update(index, value)
	if value < self.mins[i] then
		mins[i] = value
	end
	if value > maxs[i] then
		maxs[i] = value
	end
end
