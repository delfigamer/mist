local modname = ...
local node = require('index-img.node')
local load = node:module(modname)
local fmat = require('index-img.fmat')

function load:process()
	self.result = fmat:read(self.conf.file or 'input.fmat')
end
